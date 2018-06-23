// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 19:04 BRT
// Last edited on May 12 of 2018, at 15:22 BRT

#include <chicago/arch/uart.h>

#include <chicago/debug.h>

Void ArchInit(Void) {
	UARTInit();						// Init the UART (for debugging)
	DebugWriteFormated("UART initialized\r\n");
}
