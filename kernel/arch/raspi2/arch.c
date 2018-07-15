// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:18 BRT
// Last edited on July 14 of 2018, at 22:46 BRT

#include <chicago/arch/uart.h>

#include <chicago/debug.h>

UIntPtr MmGetPhys(UIntPtr addr) { (Void)addr; return 0; }
Boolean MmMap(UIntPtr virt, UIntPtr phys, UInt32 flags) { (Void)virt; (Void)phys; (Void)flags; return False; }
Boolean MmUnmap(UIntPtr virt) { (Void)virt; return 0; }

Void ArchInit(Void) {
	UARTInit();						// Init the UART (for debugging)
	DbgWriteFormated("[raspi2] UART initialized\r\n");
}
