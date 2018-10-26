// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 13:58 BRT
// Last edited on October 26 of 2018, at 20:15 BRT

#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/int.h>

#include <chicago/console.h>
#include <chicago/device.h>
#include <chicago/heap.h>
#include <chicago/file.h>

extern UInt8 ArchBootDrive;
extern UIntPtr LoaderEnd;

Int32 ArchGetMemoryMap(UInt32 buf) {
	UInt32 curr = 0;
	Int32 count = 0;
	UInt32 stack = 0x500;
	PSMAPEntry buffer = (PSMAPEntry)buf;
	PInt86Registers regs = (PInt86Registers)stack;
	
	do {
		buffer->acpi = 1;														// Force a valid ACPI 3.x entry
		regs->eax = 0xE820;														// Function 0xE820
		regs->ebx = curr;
		regs->ecx = 24;															// Ask for 24 bytes
		regs->edx = 0x534D4150;													// Place "SMAP" into EDX
		regs->edi = (UInt32)buffer;
		
		Int86(0x15, stack);														// Call INT 0x15
		
		curr = regs->ebx;
		
		if (regs->eax != 0x534D4150) {											// Check the signature
			return -1;															// Invalid...
		} else if (regs->ecx <= 20 && ((buffer->acpi & 0x01) != 0)) {			// Check if we got a 24-byte ACPI 3.x response (and if the ignore data bit is set)
			buffer++;															// Ok, we can increase the count!
			count++;
		}
	} while (curr != 0);
	
	return count;
}

Void ArchInit(Void) {
	IDTInit();																	// Init the IDT
	HeapInit((UIntPtr)&LoaderEnd, 0x7FFFF);										// Init the heap
	FsInitDeviceList();															// Init the device list
	IDEInit();																	// Init the IDE driver
	
	if (ArchBootDrive > 0x81) {													// The boot device is the CD-ROM?
		FsSetBootDevice("CdRom0");												// Yes!
	} else {
		ConWriteFormated("PANIC! Unsupported boot device\r\n");					// No, but we only support CD-ROM (for now)...
	}
}
