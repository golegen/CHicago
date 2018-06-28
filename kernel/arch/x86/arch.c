// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on June 28 of 2018, at 19:28 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>
#include <chicago/mm.h>

Void ArchInit(Void) {
	SerialInit(COM1_PORT);																								// Init debugging (using COM1 port)
	DebugWriteFormated("COM1 initialized\r\n");
	
	if (MultibootHeaderMagic != 0x2BADB002) {
		DebugWriteFormated("PANIC! We need GRUB (or any other multiboot-compilant bootloader)\r\n");
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
	DebugWriteFormated("GDT initialized\r\n");
	
	IDTInit();																											// Init the IDT
	DebugWriteFormated("IDT initialized\r\n");
	
	VMMPreInit();																										// Alloc the VMM structs
	PMMInit();																											// Init the PMM
	DebugWriteFormated("PMM initialized\r\n");
	
	VMMInit();																											// Init thhe VMM
	DebugWriteFormated("VMM initialized\r\n");
}
