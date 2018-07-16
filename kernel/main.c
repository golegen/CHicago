// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on July 16 of 2018, at 18:22 BRT

#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/file.h>

Void KernelMain(Void) {
	ArchInit();																			// Start by initalizating the architecture-dependent bits of the kernel
	DbgWriteFormated("[Kernel] Arch initialized\r\n");	
	
	FsInitDevices();																	// Init the basic devices
	DbgWriteFormated("[Kernel] Devices initialized\r\n");
	
	FsInit();																			// Init the filesystem list, mount point list, and add the basic mount points
	DbgWriteFormated("[Kernel] Filesystem initialized\r\n");
	
	while (1) ;
}
