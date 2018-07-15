// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on July 15 of 2018, at 12:28 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/registers.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/heap.h>

Void ArchInit(Void) {
	SerialInit(COM1_PORT);																								// Init debugging (using COM1 port)
	DbgWriteFormated("[x86] COM1 initialized\r\n");
	
	if (MultibootHeaderMagic != 0x2BADB002) {
		DbgWriteFormated("PANIC! We need GRUB (or any other multiboot-compilant bootloader)\r\n");
		while (1) ;
	} else {
		MultibootHeaderPointer->boot_device += 0xC0000000;																// Fix the multiboot pointer structure
		MultibootHeaderPointer->cmd_line += 0xC0000000;
		MultibootHeaderPointer->mods_address += 0xC0000000;
		MultibootHeaderPointer->address += 0xC0000000;
		MultibootHeaderPointer->mmap_address += 0xC0000000;
		MultibootHeaderPointer->drives_address += 0xC0000000;
		MultibootHeaderPointer->config_table += 0xC0000000;
		MultibootHeaderPointer->boot_loader_name += 0xC0000000;
		MultibootHeaderPointer->apm_table += 0xC0000000;
		MultibootHeaderPointer->vbe_control_info += 0xC0000000;
		MultibootHeaderPointer->vbe_mode_info += 0xC0000000;
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
	DbgWriteFormated("[x86] Devices initialized\r\n");
}
