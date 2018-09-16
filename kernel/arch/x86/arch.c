// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on September 16 of 2018, at 12:32 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/mbdisp.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pit.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/process.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/heap.h>
#include <chicago/string.h>

UIntPtr ArchBootDrive = 0;
UIntPtr ArchBootPart1 = 0;
UIntPtr ArchBootPart2 = 0;
UIntPtr ArchBootPart3 = 0;

Void ArchInitFPU(Void) {
	UInt16 cw0 = 0x37E;
	UInt16 cw1 = 0x37A;
	UIntPtr cr0;
	UIntPtr d;
	
	if (!CPUIDCheck()) {																								// Let's check if we can use the CPUID instruction
		DbgWriteFormated("PANIC! CPUID instruction isn't avaliable\r\n");												// We can't but we need it...
		while (1) ;
	}
	
	Asm Volatile("cpuid" : "=d"(d) : "a"(1) : "ecx", "ebx");															// EAX = 1, Get features
	
	if (!(d & (1 << 0))) {																								// FPU avaliable?
		DbgWriteFormated("PANIC! FPU isn't avaliable\r\n");																// Nope
		while (1) ;
	}
	
	Asm Volatile("mov %%cr0, %0" : "=r"(cr0));
	
	cr0 &= ~(1 << 3);																									// Disable the EMulation bit
	
	Asm Volatile("mov %0, %%cr0" :: "r"(cr0));
	Asm Volatile("fninit");																								// Write some initial FPU settings
	Asm Volatile("fldcw %0" :: "m"(cw0));																				// Invalid operand exceptions enabled
	Asm Volatile("fldcw %0" :: "m"(cw1));																				// Both division-by-zero and invalid operands cause exceptions
	Asm Volatile("fxsave (%0)" :: "r"(PsFPUDefaultState));																// Save the curr state (as it is the "default state")
}

Void ArchInitPMM(Void) {
	if (MultibootHeaderMagic != 0x2BADB002) {
		DbgWriteFormated("PANIC! We need GRUB (or any other multiboot-compilant bootloader)\r\n");
		while (1) ;
	} else {
		MultibootHeaderPointer->cmd_line += 0xC0000000;																	// Fix the multiboot pointer structure
		MultibootHeaderPointer->mods_address += 0xC0000000;
		MultibootHeaderPointer->address += 0xC0000000;
		MultibootHeaderPointer->mmap_address += 0xC0000000;
		MultibootHeaderPointer->drives_address += 0xC0000000;
		MultibootHeaderPointer->config_table += 0xC0000000;
		MultibootHeaderPointer->boot_loader_name += 0xC0000000;
		MultibootHeaderPointer->apm_table += 0xC0000000;
		MultibootHeaderPointer->vbe_control_info += 0xC0000000;
		MultibootHeaderPointer->vbe_mode_info += 0xC0000000;
		ArchBootDrive = (MultibootHeaderPointer->boot_device >> 24) & 0xFF;
		ArchBootPart1 = (MultibootHeaderPointer->boot_device >> 16) & 0xFF;
		ArchBootPart2 = (MultibootHeaderPointer->boot_device >> 8) & 0xFF;
		ArchBootPart3 = MultibootHeaderPointer->boot_device & 0xFF;
	}
	
	PMMInit();																											// Init the PMM
}

Void ArchInitDebug(Void) {
	SerialInit(COM1_PORT);																								// Init debugging (using COM1 port)
}

Void ArchInitDisplay(Void) {
	MultibootDisplayInit();
}

Void ArchPreInitDisplay(Void) {
	if (!MultibootDisplayPreInit()) {
		DbgWriteFormated("PANIC! Couldn't init the display\r\n");
		while (1) ;
	}
}

Void ArchInit(Void) {
	GDTInit();																											// Init the GDT
	DbgWriteFormated("[x86] GDT initialized\r\n");
	
	IDTInit();																											// Init the IDT
	DbgWriteFormated("[x86] IDT initialized\r\n");
	
	PITInit();																											// Init the PIT
	DbgWriteFormated("[x86] PIT initialized\r\n");
	
	HeapInit(KernelRealEnd, 0xFFC00000);																				// Init the kernel heap (start after all the internal kernel structs and end at the start of the temp addresses)
	DbgWriteFormated("[x86] VMM initialized\r\n");
	
	FsInitDeviceList();																									// Init the x86-only devices (and the device list)
	IDEInit();
	
	if ((ArchBootDrive >= 0xE0) && (ArchBootDrive <= 0xE9)) {															// CDROM boot value
		PChar name = StrDuplicate(IDEGetCDROMString());																	// Yes, duplicate the CdRomX string
		
		if (name == Null) {																								// Failed?
			DbgWriteFormated("[x86] Falling back boot device to CdRom0\r\n");											// Yes, so let's use the CdRom0
			FsSetBootDevice("CdRom0");
		} else {
			name[5] = (Char)((ArchBootDrive - 0xE0) + '0');																// Set the num
			FsSetBootDevice(name);																						// Try to set the device
		}
	} else if ((ArchBootDrive == 0xEF) || (ArchBootDrive == 0x9F)) {													// Other possible value for CDROM boot, but with this one we can't get the exactly boot device, let's put CdRom0
		DbgWriteFormated("[x86] Falling back boot device to CdRom0\r\n");
		FsSetBootDevice("CdRom0");
	}
	
	if (FsGetBootDevice() == Null) {
		DbgWriteFormated("PANIC! Couldn't get the boot device\r\n");
		while (1) ;
	} else {
		DbgWriteFormated("[x86] Devices initialized\r\n");
	}
}
