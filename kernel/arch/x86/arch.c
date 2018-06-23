// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on June 23 of 2018, at 12:56 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/heap-int.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/pmm-int.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>
#include <chicago/heap.h>

Void ArchInit(Void) {
	SerialInit(COM1_PORT);																					// Init debugging (using COM1 port)
	DebugWriteFormated("COM1 initialized\r\n");
	
	if (MultibootHeaderMagic != 0x2BADB002) {
		DebugWriteFormated("PANIC! We need GRUB (or any other multiboot-compilant bootloader)\r\n");
		while (1) ;
	} else {
		VMMFixMultiboot(MultibootHeaderPointer);															// Fix the multiboot header
	}
	
	GDTInit();																								// Init the GDT
	DebugWriteFormated("GDT initialized\r\n");
	
	IDTInit();																								// Init the IDT
	DebugWriteFormated("IDT initialized\r\n");
	
	KernelHeap = HeapCreate("KernelHeap", 0);																// Init the kernel heap (we're going to set the start of it later)
	DebugWriteFormated("Heap initialized\r\n");
	
	VMMPreInit();																							// Alloc VMM structs
	PMMInit();																								// Init the PMM
	DebugWriteFormated("PMM initialized\r\n");
	
	KernelHeap->start = KernelRealEnd;																		// Set the start of the kernel heap
	KernelHeap->current_position = KernelRealEnd;															// Set current position to the start of the kernel heap
	((PHeapPrivateData)(KernelHeap->private_data))->current_position_aligned = KernelRealEnd;				// And the start is by default aligned!
	
	VMMInit();																								// Init the VMM
	DebugWriteFormated("VMM initialized\r\n");
}
