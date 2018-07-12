// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:19 BRT
// Last edited on July 11 of 2018, at 12:39 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/serial.h>

#include <chicago/debug.h>

UIntPtr MmGetPhys(UIntPtr addr) { (Void)addr; return 0; }
Boolean MmMap(UIntPtr virt, UIntPtr phys, UInt32 flags) { (Void)virt; (Void)phys; (Void)flags; return False; }
Boolean MmUnmap(UIntPtr virt) { (Void)virt; return 0; }

Void ArchInit(Void) {
	SerialInit(COM1_PORT);		// Init debugging (using COM1 port)
	DbgWriteFormated("COM1 initialized\r\n");
	
	GDTInit();					// Init the GDT
	DbgWriteFormated("GDT initialized\r\n");
	
	IDTInit();					// Init the IDT
	DbgWriteFormated("IDT initialized\r\n");
}
