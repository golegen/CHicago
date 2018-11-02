// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on November 02 of 2018, at 17:28 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/file.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/version.h>

Void KernelMain(Void) {
	ArchInitDebug();																										// Init the architecture-dependent debugging method
	DbgWriteFormated("[Kernel] Arch debugging initialized\r\n");
	
	DbgWriteFormated("[Kernel] CHicago %s (codename %s, for %s)\r\n", CHICAGO_VSTR, CHICAGO_CODENAME, CHICAGO_ARCH);		// Print the system version
	
	ArchInitFPU();																											// Init the architecture-dependent FPU (floating point unit)
	DbgWriteFormated("[Kernel] Arch FPU initialized\r\n");
	
	ArchInitPMM();																											// Init the physical memory manager
	DbgWriteFormated("[Kernel] Arch PMM initialized\r\n");
	
	ArchInitVMM();																											// Init the virtual memory manager
	DbgWriteFormated("[Kernel] Arch VMM initialized\r\n");
	
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
	
	ConClearScreen();																										// Clear the screen
	ConWriteFormated("CHicago Operating System for %s\r\n", CHICAGO_ARCH);													// Print some system informations
	ConWriteFormated("Codename '%s'\r\n", CHICAGO_CODENAME);
	ConWriteFormated("%s\r\n\r\n", CHICAGO_VSTR);
	
	if (!FsMountFile("\\MountPoint", "\\Devices\\HardDisk0", Null)) {														// Try to mount the hard disk
		ConWriteFormated("Couldn't mount \\Devices\\HardDisk0\r\n");
		goto l;
	}
	
	PFsNode file = FsOpenFile("\\MountPoint\\hello");																		// Try to open the hello (test) file
	
	if (file == Null) {
		ConWriteFormated("Couldn't open \\MountPoint\\hello\r\n");
		goto l;
	}
	
	PUInt8 buf = (PUInt8)MemAllocate(file->length);																			// Try to alloc space for reading it
	
	if (buf == Null) {
		ConWriteFormated("Couldn't alloc space for reading the file\r\n");
		goto l;
	}
	
	if (!FsReadFile(file, 0, file->length, buf)) {																			// Try to read it
		ConWriteFormated("Couldn't read the file\r\n");
		goto l;
	}
	
	ConWriteFormated("Length: %d\r\n", file->length);																		// Show the length of the file
	ConWriteFormated("Data: %s\r\n", buf);																					// Show the data
	
	if ((file->length != 20) && (!StrCompareMemory(buf, "Hello, write world!\0", 20))) {									// And test the write function!
		StrCopyMemory(buf, "Hello, write world!\0", 20);
		
		if (!FsWriteFile(file, 0, 20, buf)) {
			ConWriteFormated("Couldn't write to the file\r\n");
			goto l;
		}
	}
	
l:	while (1) ;
}
