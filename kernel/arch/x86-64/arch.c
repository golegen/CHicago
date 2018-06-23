// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:19 BRT
// Last edited on May 27 of 2018, at 12:00 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/serial.h>

#include <chicago/debug.h>

Void ArchInit(Void) {
	SerialInit(COM1_PORT);		// Init debugging (using COM1 port)
	DebugWriteFormated("COM1 initialized\r\n");
	
	GDTInit();					// Init the GDT
	DebugWriteFormated("GDT initialized\r\n");
	
	IDTInit();					// Init the IDT
	DebugWriteFormated("IDT initialized\r\n");
}
