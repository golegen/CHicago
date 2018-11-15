// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 13:58 BRT
// Last edited on November 15 of 2018, at 15:57 BRT

#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/int.h>

#include <chicago/alloc.h>
#include <chicago/console.h>
#include <chicago/device.h>
#include <chicago/heap.h>
#include <chicago/file.h>
#include <chicago/string.h>

extern UInt8 ArchBootDrive;
extern UIntPtr LoaderEnd;

Int32 ArchGetMemoryMap(Void) {
	UInt32 curr = 0;
	Int32 count = 0;
	UInt32 stack = 0x500;
	PSMAPEntry buffer = (PSMAPEntry)0x3000;
	PInt86Registers regs = (PInt86Registers)stack;
	
	do {
		buffer->acpi = 1;																		// Force a valid ACPI 3.x entry
		regs->eax = 0xE820;																		// Function 0xE820
		regs->ebx = curr;
		regs->ecx = 24;																			// Ask for 24 bytes
		regs->edx = 0x534D4150;																	// Place "SMAP" into EDX
		regs->edi = (UInt32)buffer;																// Set the buffer
		
		Int86(0x15, stack);																		// Call INT 0x15
		
		curr = regs->ebx;
		
		if (regs->eax != 0x534D4150) {															// Check the signature
			return -1;																			// Invalid...
		} else if (regs->ecx <= 20 && ((buffer->acpi & 0x01) != 0)) {							// Check if we got a 24-byte ACPI 3.x response (and if the ignore data bit is set)
			buffer++;																			// Ok, we can increase the count!
			count++;
		}
	} while (curr != 0);
	
	return count;
}

Int16 ArchGetVesaMode(UInt16 width, UInt16 height, UInt8 bpp, PUInt32 fb) {
	UInt32 stack = 0x500;
	PVBEInfoBlock info = (PVBEInfoBlock)0x1000;
	PInt86Registers regs = (PInt86Registers)stack;
	
	regs->eax = 0x4F00;																			// Function 0x4F00
	regs->edi = (UInt32)info;																	// Set the buffer
	
	Int86(0x10, stack);																			// Call INT 0x10
	
	if (regs->eax != 0x4F) {																	// Error?
		return -1;																				// Yes...
	}
	
	for (UInt16 i = 0; info->modes[i] != 0xFFFF; i++) {											// Let's iterate the list!
		PVBEModeInfoBlock inf = (PVBEModeInfoBlock)0x2000;
		
		regs->eax = 0x4F01;																		// Function 0x4F01
		regs->ecx = info->modes[i];																// Set the mode
		regs->edi = (UInt32)inf;																// Set the buffer
		
		Int86(0x10, stack);																		// Call INT 0x10
		
		if (regs->eax != 0x4F) {																// Failed?
			return -1;																			// Yes...
		} else if ((inf->attrs & 0x90) != 0x90) {												// Linear Frame Buffer support?
			continue;																			// No...
		} else if (inf->memmodel != 0x04 && inf->memmodel != 0x06) {							// Packed pixel or Direct color mode?
			continue;																			// No...
		} else if (inf->width != width || inf->height != height || inf->bpp != bpp) {			// Found?
			continue;																			// No
		}
		
		if (fb != Null) {																		// FOUND! Save the physical framebuffer address?
			*fb = inf->phys;																	// Yes!
		}
		
		return info->modes[i];
	}
	
	return -1;
}

Boolean ArchSetVesaMode(UInt16 width, UInt16 height, UInt8 bpp, PUInt32 data) {
	UInt32 fb = 0;
	UInt32 stack = 0x500;
	PInt86Registers regs = (PInt86Registers)stack;
	Int16 mod = ArchGetVesaMode(width, height, bpp, &fb);										// Get the mode num
	
	if (mod == -1) {
		return False;																			// Failed
	}
	
	regs->eax = 0x4F02;																			// Function 0x4F02
	regs->ebx = 0x4000 | mod;																	// ALWAYS ENABLE THE LFB (Linear Frame Buffer) BIT
	
	Int86(0x10, stack);																			// Call INT 0x10
	
	if (regs->eax != 0x4F) {																	// Failed?
		return False;																			// Yes...
	}
	
	if (data != Null) {																			// Set out data?
		data[0] = width;																		// Yes!
		data[1] = height;
		data[2] = bpp;
		data[3] = fb;
	}
	
	return True;
}

IntPtr ArchJump(UIntPtr dest, PChar bootdev) {
	if (dest == 0 || bootdev == Null) {															// Valid dest and boot device?
		return -1;																				// Nope
	}
	
	Int32 mmapc = ArchGetMemoryMap();															// Get the memory map
	
	if (mmapc == -1) {
		return -1;																				// Failed
	}
	
	UInt32 data[4];
	
	if (ArchSetVesaMode(1366, 768, 32, data)) {													// Set the VESA mode to 1366x768x32
		goto c;																					// Ok
	} else if (ArchSetVesaMode(1366, 768, 24, data)) {											// Set the VESA mode to 1366x768x24
		goto c;																					// Ok
	} else if (ArchSetVesaMode(1280, 720, 32, data)) {											// Set the VESA mode to 1280x720x32
		goto c;																					// Ok!
	} else if (ArchSetVesaMode(1280, 720, 24, data)) {											// Set the VESA mode to 1280x720x24
		goto c;																					// Ok!
	} else if (ArchSetVesaMode(800, 600, 32, data)) {											// Set the VESA mode to 800x600x32
		goto c;																					// Ok!
	} else if (ArchSetVesaMode(800, 600, 24, data)) {											// Set the VESA mode to 800x600x24
		goto c;																					// Ok!
	} else {																					// Couldn't any supported VESA mode...
		return -2;
	}
	
c:	ArchJumpInt(dest, bootdev, 0x3000, mmapc, data);											// Jump!
	
	return -1;																					// ... Returned?
}

Void ArchHalt(Void) {
	while (True) {
		Asm Volatile("cli; hlt");																// Disable interrupts and Halt!
	}
}

Void ArchInit(Void) {
	IDTInit();																					// Init the IDT
	HeapInit((UIntPtr)&LoaderEnd, 0x7FFFF);														// Init the heap
	FsInitDeviceList();																			// Init the device list
	IDEInit();																					// Init the IDE driver
	
	if (ArchBootDrive > 0x81) {																	// The boot device is the CD-ROM?
		FsSetBootDevice("CdRom0");																// Yes!
	} else {
		ConWriteFormated("PANIC! Unsupported boot device\r\n");									// No, but we only support CD-ROM (for now)...
	}
}
