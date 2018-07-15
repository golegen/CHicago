// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on July 15 of 2018, at 13:24 BRT

#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/device.h>

Void KernelMain(Void) {
	ArchInit();																			// Start by initalizating the architecture-dependent bits of the kernel
	DbgWriteFormated("[Kernel] Arch initialized\r\n");	
	
	NullDeviceInit();																	// Add the Null device
	ZeroDeviceInit();																	// Add the Zero device
	DbgWriteFormated("[Kernel] Devices initialized\r\n");
	
	FsDbgListDevices();
	
	while (1) ;
}
