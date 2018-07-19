// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 15:28 BRT
// Last edited on July 19 of 2018, at 02:25 BRT

#include <chicago/arch/port.h>
#include <chicago/arch/vga.h>

#include <chicago/display.h>

UInt8 VGARegisters[] = {
	0x63, 0x03, 0x01, 0x0F, 0x00,
	0x0E, 0x5F, 0x4F, 0x50, 0x82,
	0x54, 0x80, 0xBF, 0x1F, 0x00,
	0x41, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x9C, 0x0E, 0x8F,
	0x28, 0x40, 0x96, 0xB9, 0xA3,
	0xFF, 0x00, 0x00, 0x00, 0x00,
	0x40, 0x05, 0x0F, 0xFF, 0x00,
	0x01, 0x02, 0x03, 0x04, 0x05,
	0x06, 0x07, 0x08, 0x09, 0x0A,
	0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

Void VGAPreInit(Void) {
	PUInt8 curr = VGARegisters;
	
	PortOutByte(VGA_MISC_WRITE, *curr++);								// First, let's write the miscellaneous reg
	
	for (UIntPtr i = 0; i < VGA_NUM_SEQ_REGS; i++) {					// Now the sequencer regs
		PortOutByte(VGA_SEQ_INDEX, i);
		PortOutByte(VGA_SEQ_DATA, *curr++);
	}
	
	PortOutByte(VGA_CRTC_INDEX, 0x03);									// Now let's unlock the crtc regs
	PortOutByte(VGA_CRTC_DATA, PortInByte(VGA_CRTC_DATA) | 0x80);
	PortOutByte(VGA_CRTC_INDEX, 0x11);
	PortOutByte(VGA_CRTC_DATA, PortInByte(VGA_CRTC_DATA) & ~0x80);
	
	VGARegisters[0x03] |= 0x80;											// Make sure that they will remain unlocked
	VGARegisters[0x11] &= ~0x80;
	
	for (UInt32 i = 0; i < VGA_NUM_CRTC_REGS; i++) {					// Now, write the crtc regs!
		PortOutByte(VGA_CRTC_INDEX, i);
		PortOutByte(VGA_CRTC_DATA, *curr++);
	}
	
	for (UInt32 i = 0; i < VGA_NUM_GC_REGS; i++) {						// Write the graphics controller regs
		PortOutByte(VGA_GC_INDEX, i);
		PortOutByte(VGA_GC_DATA, *curr++);
	}
	
	for (UInt32 i = 0; i < VGA_NUM_AC_REGS; i++) {						// And the attribute controller regs
		PortInByte(VGA_INSTAT_READ);
		PortOutByte(VGA_AC_INDEX, i);
		PortOutByte(VGA_AC_WRITE, *curr++);
	}
	
	PortInByte(VGA_INSTAT_READ);										// Lock the 16-color palette and unblank the display
	PortOutByte(VGA_AC_INDEX, 0x20);
	PortOutByte(VGA_DAC_WRITE_INDEX, 0x00);								// The DAC is still in an undefined state, so let's setup it, so we have a guaranteed set of colors
	
	for (UInt32 i = 0; i < 4; i++) {
		PortOutByte(VGA_DAC_DATA, 0x3F);
	}
	
	DispPreInit(320, 200, 1);											// Call the DispPreInit function to create the framebuffer space!
}

Void VGAInit(Void) {
	DispInit(VGA_FRAMEBUFFER);											// Call the DispInit function to map the framebuffer space!
}
