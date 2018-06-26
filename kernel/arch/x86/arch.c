// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on June 26 of 2018, at 19:31 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/pmm-int.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>
#include <chicago/heap.h>

Void ArchInit(Void) {
	SerialInit(COM1_PORT);																								// Init debugging (using COM1 port)
	DebugWriteFormated("COM1 initialized\r\n");
	
	if (MultibootHeaderMagic != 0x2BADB002) {
		DebugWriteFormated("PANIC! We need GRUB (or any other multiboot-compilant bootloader)\r\n");
		while (1) ;
	} else {
		VMMFixMultiboot(MultibootHeaderPointer);																		// Fix the multiboot header
	}
	
	GDTInit();																											// Init the GDT
	DebugWriteFormated("GDT initialized\r\n");
	
	IDTInit();																											// Init the IDT
	DebugWriteFormated("IDT initialized\r\n");
	
	VMMPreInit();
	PMMInit();																											// Init the PMM
	DebugWriteFormated("PMM initialized\r\n");
	
	VMMInit();																											// Init the VMM
	HeapInit();																											// Init the kernel heap
	DebugWriteFormated("VMM initialized\r\n\r\n");
}
