// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 13:58 BRT
// Last edited on October 24 of 2018, at 18:03 BRT

#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>

#include <chicago/console.h>
#include <chicago/device.h>
#include <chicago/heap.h>
#include <chicago/file.h>

extern UInt8 ArchBootDrive;
extern UIntPtr LoaderEnd;

Void ArchInit(Void) {
	IDTInit();														// Init the IDT
	HeapInit((UIntPtr)&LoaderEnd, 0x7FFFF);							// Init the heap
	FsInitDeviceList();												// Init the device list
	IDEInit();														// Init the IDE driver
	
	if (ArchBootDrive > 0x81) {										// The boot device is the CD-ROM?
		FsSetBootDevice("CdRom0");									// Yes!
	} else {
		ConWriteFormated("PANIC! Unsupported boot device\r\n");		// No, but we only support CD-ROM (for now)...
	}
}
