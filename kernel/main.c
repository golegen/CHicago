// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on July 22 of 2018, at 14:12 BRT

#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/display.h>
#include <chicago/file.h>

Void KernelMain(Void) {
	ArchInitDebug();																	// Init the architecture-dependent debugging method
	DbgWriteFormated("[Kernel] Arch debugging initialized\r\n");
	
	ArchInitFPU();																		// Init the architecture-dependent FPU (floating point unit)
	DbgWriteFormated("[Kernel] Arch FPU initialized\r\n");
	
	ArchPreInitDisplay();																// Pre init the display
	DbgWriteFormated("[Kernel] Arch display pre-initialized\r\n");
	
	ArchInitPMM();																		// Init the physical memory manager (free all the... free? memory regions)
	DbgWriteFormated("[Kernel] Arch PMM initialized\r\n");
	
	ArchInitDisplay();																	// Init the display
	DbgWriteFormated("[Kernel] Arch display initialized\r\n");
	
	DispDrawProgessBar();																// Draw the progress bar
	DbgWriteFormated("[Kernel] The boot progress bar has been shown\r\n");
	
	ArchInit();																			// Let's finish it by initalizating the architecture-dependent bits of the kernel
	DispDrawProgessBar();
	DbgWriteFormated("[Kernel] Arch initialized\r\n");	
	
	FsInitDevices();																	// Now init the basic devices
	DispDrawProgessBar();
	DbgWriteFormated("[Kernel] Devices initialized\r\n");
	
	FsInit();																			// Init the filesystem list, mount point list, and add the basic mount points
	DispDrawProgessBar();
	DbgWriteFormated("[Kernel] Filesystem initialized\r\n");
	
	DispFillProgressBar();																// BOOT PROCESS, FINISHED!
	DbgWriteFormated("[Kernel] Boot process finished\r\n");
	
	while (1) ;
}
