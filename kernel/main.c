// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on September 15 of 2018, at 13:20 BRT

#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/file.h>
#include <chicago/process.h>
#include <chicago/version.h>

Void KernelMain(Void) {
	ArchInitDebug();																																				// Init the architecture-dependent debugging method
	DbgWriteFormated("[Kernel] Arch debugging initialized\r\n");
	
	DbgWriteFormated("[Kernel] CHicago %d.%d build %d (codename %s, for %s)\r\n",CHICAGO_MAJOR, CHICAGO_MINOR, CHICAGO_BUILD, CHICAGO_CODENAME, CHICAGO_ARCH);		// Print the system version
	
	ArchInitFPU();																																					// Init the architecture-dependent FPU (floating point unit)
	DbgWriteFormated("[Kernel] Arch FPU initialized\r\n");
	
	ArchPreInitDisplay();																																			// Pre init the display
	DbgWriteFormated("[Kernel] Arch display pre-initialized\r\n");
	
	ArchInitPMM();																																					// Init the physical memory manager (free all the... free? memory regions)
	DbgWriteFormated("[Kernel] Arch PMM initialized\r\n");
	
	ArchInitDisplay();																																				// Init the display
	DbgWriteFormated("[Kernel] Arch display initialized\r\n");
	
	DispDrawProgessBar();																																			// Draw the progress bar
	DbgWriteFormated("[Kernel] The boot progress bar has been shown\r\n");
	
	ArchInit();																																						// Let's finish it by initalizating the architecture-dependent bits of the kernel
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Arch initialized\r\n");	
	
	FsInitDevices();																																				// Now init the basic devices
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Devices initialized\r\n");
	
	FsInit();																																						// Init the filesystem list, mount point list, and add the basic mount points
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Filesystem initialized\r\n");
	
	PsInit();																																						// Init tasking
	
	while (1) ;
}

Void KernelMainLate(Void) {
	PsTaskSwitchEnabled = True;																																		// Enable task switch
	DbgWriteFormated("[Kernel] Tasking initialized\r\n");
	
	DispFillProgressBar();																																			// BOOT PROCESS, FINISHED!
	DbgWriteFormated("[Kernel] Boot process finished\r\n");
	
	while (1) ;
}
