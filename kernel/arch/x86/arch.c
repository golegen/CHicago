// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on July 15 of 2018, at 14:40 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/registers.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/heap.h>

Void ArchInit(Void) {
	UIntPtr bddrive = 0;
	UIntPtr bdpart1 = 0;
	UIntPtr bdpart2 = 0;
	UIntPtr bdpart3 = 0;
	
	SerialInit(COM1_PORT);																								// Init debugging (using COM1 port)
	DbgWriteFormated("[x86] COM1 initialized\r\n");
	
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
		bddrive = (MultibootHeaderPointer->boot_device >> 24) & 0xFF;
		bdpart1 = (MultibootHeaderPointer->boot_device >> 16) & 0xFF;
		bdpart2 = (MultibootHeaderPointer->boot_device >> 8) & 0xFF;
		bdpart3 = MultibootHeaderPointer->boot_device & 0xFF;
	}
	
	GDTInit();																											// Init the GDT
	DbgWriteFormated("[x86] GDT initialized\r\n");
	
	IDTInit();																											// Init the IDT
	DbgWriteFormated("[x86] IDT initialized\r\n");
	
	PMMInit();																											// Init the PMM
	DbgWriteFormated("[x86] PMM initialized\r\n");
	
	HeapInit(KernelRealEnd, 0xFFC00000);																				// Init the kernel heap (start after all the internal kernel structs and end at the start of the temp addresses)
	DbgWriteFormated("[x86] VMM initialized\r\n");
	
	FsInitDevices();																									// Init the x86-only devices (and the device list)
	IDEInit();
	
	if ((bddrive >= 0x80) && (bddrive <= 0x89) && (bdpart1 == 0) && (bdpart2 == 0) && (bdpart3 == 0)) {					// RAW Hard Disk?
		PChar name = (PChar)MemAllocate(10);																			// Allocate space for the name
		PChar hname = IDEGetHardDiskString();
		
		if (name == Null) {																								// Failed?
			DbgWriteFormated("[x86] Falling back boot device to HardDisk0\r\n");										// Yes, so let's use the HardDisk0
			FsSetBootDevice("HardDisk0");
		} else {
			for (UIntPtr i = 0; i < 10; i++) {																			// Copy the HardDisk base dev name
				name[i] = hname[i];
			}
			
			name[8] = (Char)((bddrive - 0x80) + '0');																	// Set the CDROM num
			
			FsSetBootDevice(name);																						// Set the device
		}
	} else if ((bddrive >= 0xE0) && (bddrive <= 0xE9)) {																// CDROM boot value
		PChar name = (PChar)MemAllocate(7);																				// Allocate space for the name
		PChar cname = IDEGetCDROMString();
		
		if (name == Null) {																								// Failed?
			DbgWriteFormated("[x86] Falling back boot device to CdRom0\r\n");											// Yes, so let's use the CdRom0
			FsSetBootDevice("CdRom0");
		} else {
			for (UIntPtr i = 0; i < 7; i++) {																			// Copy the CDROM base dev name
				name[i] = cname[i];
			}
			
			name[5] = (Char)((bddrive - 0xE0) + '0');																	// Set the CDROM num
			
			FsSetBootDevice(name);																						// Set the device
		}
	} else if (bddrive == 0x9F) {																						// Other possible value for CDROM boot, but with this one we can't get the exactly boot device, let's put CdRom0
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
