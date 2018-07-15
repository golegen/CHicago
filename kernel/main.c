// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on July 14 of 2018, at 22:48 BRT

#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/device.h>

Void KernelMain(Void) {
	ArchInit();																			// Start by initalizating the architecture-dependent bits of the kernel
	DbgWriteFormated("[Kernel] Arch initialized\r\n");	
	
	FsDbgListDevices();
	
	while (1) ;
}
