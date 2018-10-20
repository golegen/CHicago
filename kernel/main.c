// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on October 19 of 2018, at 21:14 BRT

#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/file.h>
#include <chicago/gui.h>
#include <chicago/mm.h>
#include <chicago/process.h>
#include <chicago/version.h>
#include <chicago/virt.h>

Void KernelMain(Void) {
	ArchInitDebug();																										// Init the architecture-dependent debugging method
	DbgWriteFormated("[Kernel] Arch debugging initialized\r\n");
	
	DbgWriteFormated("[Kernel] CHicago %s (codename %s, for %s)\r\n", CHICAGO_VSTR, CHICAGO_CODENAME, CHICAGO_ARCH);		// Print the system version
	
	ArchInitFPU();																											// Init the architecture-dependent FPU (floating point unit)
	DbgWriteFormated("[Kernel] Arch FPU initialized\r\n");
	
	ArchPreInitDisplay();																									// Pre init the display
	DbgWriteFormated("[Kernel] Arch display pre-initialized\r\n");
	
	ArchInitPMM();																											// Init the physical memory manager (free all the... free? memory regions)
	DbgWriteFormated("[Kernel] Arch PMM initialized\r\n");
	
	ArchInitDisplay();																										// Init the display
	DbgWriteFormated("[Kernel] Arch display initialized\r\n");
	
	ArchInitMouse();																										// Init the mouse
	DbgWriteFormated("[Kernel] Arch mouse initialized\r\n");
	
	ArchInitKeyboard();																										// Init the keyboard
	DbgWriteFormated("[Kernel] Arch keyboard intialized\r\n");
	
	DispDrawProgessBar();																									// Draw the progress bar
	DbgWriteFormated("[Kernel] The boot progress bar has been shown\r\n");
	
	ArchInit();																												// Let's finish it by initalizating the other architecture-dependent bits of the kernel
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Arch initialized\r\n");	
	
	FsInitDevices();																										// Now init the basic devices
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Devices initialized\r\n");
	
	FsInit();																												// Init the filesystem list, mount point list, and add the basic mount points
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Filesystem initialized\r\n");
	
	PsInit();																												// Init tasking
	
	while (1) ;
}

Void KernelMainLate(Void) {
	PsTaskSwitchEnabled = True;																								// Enable task switch
	DbgWriteFormated("[Kernel] Tasking initialized\r\n");
	
	DispFillProgressBar();																									// Kernel initialized :)
	DbgWriteFormated("[Kernel] Kernel initialized\r\n");
	
	GuiInit();
	DbgWriteFormated("[Gui] Gui subsystem initialized\r\n");
	
	GuiAddWindow(GuiCreateWindow("Test", 5, 5, 200, 200));																	// Show a test window
	
	while (1) ;
}
