// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:12 BRT
// Last edited on July 20 of 2018, at 19:05 BRT

#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/string.h>

UIntPtr DispFrameBuffer = 0;
UIntPtr DispWidth = 0;
UIntPtr DispHeight = 0;
UIntPtr DispBytesPerPixel = 0;

UIntPtr DispPalette256[256] = {
	0x000000,
	0x00002A,
	0x002A00,
	0x002A2A,
	0x2A0000,
	0x2A002A,
	0x2A1500,
	0x2A2A2A,
	0x151515,
	0x15153F,
	0x153F15,
	0x153F3F,
	0x3F1515,
	0x3F153F,
	0x3F3F15,
	0x3F3F3F,
	0x000000,
	0x050505,
	0x080808,
	0x0B0B0B,
	0x0E0E0E,
	0x111111,
	0x161616,
	0x181818,
	0x1C1C1C,
	0x202020,
	0x242424,
	0x282828,
	0x2D2D2D,
	0x323232,
	0x383838,
	0x3F3F3F,
	0x00003F,
	0x20003F,
	0x1F003F,
	0x2F003F,
	0x3F003F,
	0x3F002F,
	0x3F001F,
	0x3F0020,
	0x3F0000,
	0x3F2000,
	0x3F1F00,
	0x3F2F00,
	0x3F3F00,
	0x2F3F00,
	0x1F3F00,
	0x203F00,
	0x003F00,
	0x003F20,
	0x003F1F,
	0x003F2F,
	0x003F3F,
	0x002F3F,
	0x001F3F,
	0x00203F,
	0x1F1F3F,
	0x271F3F,
	0x2F1F3F,
	0x371F3F,
	0x3F1F3F,
	0x3F1F37,
	0x3F1F2F,
	0x3F1F27,
	0x3F1F1F,
	0x3F271F,
	0x3F2F1F,
	0x3F371F,
	0x3F3F1F,
	0x373F1F,
	0x2F3F1F,
	0x273F1F,
	0x1F3F1F,
	0x1F3F27,
	0x1F3F2F,
	0x1F3F37,
	0x1F3F3F,
	0x1F373F,
	0x1F2F3F,
	0x1F273F,
	0x2D2D3F,
	0x312D3F,
	0x362D3F,
	0x3A2D3F,
	0x3F2D3F,
	0x3F2D3A,
	0x3F2D36,
	0x3F2D31,
	0x3F2D2D,
	0x3F312D,
	0x3F362D,
	0x3F3A2D,
	0x3F3F2D,
	0x3A3F2D,
	0x363F2D,
	0x313F2D,
	0x2D3F2D,
	0x2D3F31,
	0x2D3F36,
	0x2D3F3A,
	0x2D3F3F,
	0x2D3A3F,
	0x2D363F,
	0x2D313F,
	0x00001C,
	0x07001C,
	0x0E001C,
	0x15001C,
	0x1C001C,
	0x1C0015,
	0x1C000E,
	0x1C0007,
	0x1C0000,
	0x1C0700,
	0x1C0E00,
	0x1C1500,
	0x1C1C00,
	0x151C00,
	0x0E1C00,
	0x071C00,
	0x001C00,
	0x001C07,
	0x001C0E,
	0x001C15,
	0x001C1C,
	0x00151C,
	0x000E1C,
	0x00071C,
	0x0E0E1C,
	0x110E1C,
	0x150E1C,
	0x180E1C,
	0x1C0E1C,
	0x1C0E18,
	0x1C0E15,
	0x1C0E11,
	0x1C0E0E,
	0x1C110E,
	0x1C150E,
	0x1C180E,
	0x1C1C0E,
	0x181C0E,
	0x151C0E,
	0x111C0E,
	0x0E1C0E,
	0x0E1C11,
	0x0E1C15,
	0x0E1C18,
	0x0E1C1C,
	0x0E181C,
	0x0E151C,
	0x0E111C,
	0x16161C,
	0x16161C,
	0x18161C,
	0x1A161C,
	0x1C161C,
	0x1C161A,
	0x1C1618,
	0x1C1616,
	0x1C1616,
	0x1C1616,
	0x1C1816,
	0x1C1A16,
	0x1C1C16,
	0x1A1C16,
	0x181C16,
	0x161C16,
	0x161C16,
	0x161C16,
	0x161C18,
	0x161C1A,
	0x161C1C,
	0x161A1C,
	0x16181C,
	0x16161C,
	0x000020,
	0x040020,
	0x080020,
	0x0C0020,
	0x200020,
	0x20000C,
	0x200008,
	0x200004,
	0x200000,
	0x200400,
	0x200800,
	0x200C00,
	0x202000,
	0x0C2000,
	0x082000,
	0x042000,
	0x002000,
	0x002004,
	0x002008,
	0x00200C,
	0x002020,
	0x000C20,
	0x000820,
	0x000420,
	0x080820,
	0x0A0820,
	0x0C0820,
	0x0E0820,
	0x200820,
	0x20080E,
	0x20080C,
	0x20080A,
	0x200808,
	0x200A08,
	0x200C08,
	0x200E08,
	0x202008,
	0x0E2008,
	0x0C2008,
	0x0A2008,
	0x082008,
	0x08200A,
	0x08200C,
	0x08200E,
	0x082020,
	0x080E20,
	0x080C20,
	0x080A20,
	0x0B0B20,
	0x0C0B20,
	0x0D0B20,
	0x0F0B20,
	0x200B20,
	0x200B0F,
	0x200B0D,
	0x200B0C,
	0x200B0B,
	0x200C0B,
	0x200D0B,
	0x200F0B,
	0x20200B,
	0x0F200B,
	0x0D200B,
	0x0C200B,
	0x0B200B,
	0x0B200C,
	0x0B200D,
	0x0B200F,
	0x0B2020,
	0x0B0F20,
	0x0B0D20,
	0x0B0C20,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
};

UIntPtr DispGetFrameBuffer(Void) {
	return DispFrameBuffer;
}

UIntPtr DispGetWidth(Void) {
	return DispWidth;
}

UIntPtr DispGetHeight(Void) {
	return DispHeight;
}

UIntPtr DispGetBytesPerPixel(Void) {
	return DispBytesPerPixel;
}

PUIntPtr DispGetPalette256(Void) {
	return DispPalette256;
}

UIntPtr DispMatchColors(UInt8 ar, UInt8 ag, UInt8 ab, UInt8 br, UInt8 bg, UInt8 bb) {
	UIntPtr diff = 0;
	Int rd = ar - br;																								// Get the diff between the colors					
	Int gd = ag - bg;
	Int bd = ab - bb;
	
	if (rd < 0) {																									// Add them, but first convert them to positive numbers!
		diff += -rd;
	} else {
		diff += rd;
	}
	
	if (gd < 0) {
		diff += -gd;
	} else {
		diff += gd;
	}
	
	if (bd < 0) {
		diff += -bd;
	} else {
		diff += bd;
	}
	
	return diff;
}

Void DispExtractRGB(UIntPtr c, PUInt8 r, PUInt8 g, PUInt8 b) {
	if (r != Null) {
		*r = (UInt8)((c >> 16) & 0xFF);
	}
	
	if (g != Null) {
		*g = (UInt8)((c >> 8) & 0xFF);
	}
	
	if (b != Null) {
		*b = (UInt8)(c & 0xFF);
	}
}

UInt8 DispConvertTo256(UIntPtr c) {
	UIntPtr bestm = (UIntPtr)-1;
	UInt8 bestc = 0;
	UInt8 ar;
	UInt8 ag;
	UInt8 ab;
	
	DispExtractRGB(c, &ar, &ag, &ab);																				// Extract the RGB values from the color
	
	for (UIntPtr i = 0; i < 256; i++) {
		UInt8 br;
		UInt8 bg;
		UInt8 bb;
		
		DispExtractRGB(DispPalette256[i], &br, &bg, &bb);															// Extract the RGB values from this palette entry
		
		UIntPtr m = DispMatchColors(ar, ag, ab, br, bg, bb);														// Lets see if they match
		
		if (m < bestm) {																							// Match better than the last one?
			bestm = m;																								// Yes, so update!
			bestc = i;
		}
		
		if (m == 0) {																								// Perfect match?
			break;																									// YEEEEESSSSS!
		}
	}
	
	return bestc;
}

Void DispPutPixel(UIntPtr x, UIntPtr y, UIntPtr c) {
	if (x >= DispWidth) {																							// Fix the x and the y if they are bigger than the screen dimensions
		x = DispWidth - 1;
	}
	
	if (y >= DispHeight) {
		y = DispHeight - 1;
	}
	
	PUInt8 fb = (PUInt8)(DispFrameBuffer + (y * (DispWidth * DispBytesPerPixel)) + (x * DispBytesPerPixel));		// Get a pointer to the framebuffer STARTING AT X, Y POSITION
	
	if (DispBytesPerPixel == 1) {																					// 1 bytes per pixel (VGA)?
		*fb = DispConvertTo256(c);																					// Yes, so we only need to convert our RGB color to the closest VGA one!
	} else if (DispBytesPerPixel == 3) {																			// 4 bytes per pixel (True Color)?
		UInt8 r;																									// Yes
		UInt8 g;
		UInt8 b;
		
		DispExtractRGB(c, &r, &g, &b);																				// Extract the RGB values
		
		*fb++ = b;																									// And write them!
		*fb++ = g;
		*fb++ = r;
	}
}

Void DispFillRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c) {
	if (x >= DispWidth) {																							// Fix the x and the y if they are bigger than the screen dimensions
		x = DispWidth - 1;
	}
	
	if (y >= DispHeight) {
		y = DispHeight - 1;
	}
	
	if ((x + w) >= DispWidth) {																						// Fix the width and height of our rectangle if they are bigger than the screen dimensions
		w = (w + x) - DispWidth;
	}
	
	if ((y + h) >= DispHeight) {
		h = (y + h) - DispHeight;
	}
	
	PUInt8 fb = (PUInt8)(DispFrameBuffer + (y * (DispWidth * DispBytesPerPixel)) + (x * DispBytesPerPixel));		// Get a pointer to the framebuffer STARTING AT X, Y POSITION
	
	if (DispBytesPerPixel == 1) {																					// 1 bytes per pixel (VGA)?
		UInt8 ci = DispConvertTo256(c);																				// Yes, so let's convert our RGB color to the closest VGA one
		
		for (UIntPtr i = 0; i < h; i++) {																			// And fill our rectangle
			for (UIntPtr j = 0; j < w; j++) {
				*fb++ = ci;
			}
			
			fb += DispWidth - w;
		}
	} else if (DispBytesPerPixel == 3) {																			// 3 bytes per pixel (True Color)?
		UInt8 r;																									// Yes
		UInt8 g;
		UInt8 b;
		
		DispExtractRGB(c, &r, &g, &b);																				// Extract the RGB values
		
		for (UIntPtr i = 0; i < h; i++) {																			// And fill our rectangle
			for (UIntPtr j = 0; j < w; j++) {
				*fb++ = b;
				*fb++ = g;
				*fb++ = r;
			}
			
			fb += (DispWidth - w) * DispBytesPerPixel;
		}
	}
}

Void DispPreInit(UIntPtr w, UIntPtr h, UIntPtr bpp) {
	if ((bpp != 1) && (bpp != 3)) {																					// For now, we only support VGA (256 colors, 1 byte per pixel) and True Color (... a lot of colors, 3 bytes per pixel)
		DbgWriteFormated("PANIC! Couldn't init the display\r\n");
		while (1); 
	}
	
	DispFrameBuffer = MmBootAlloc(w * h * bpp, True);																// Alloc some virt space for the frame buffer
	DispWidth = w;
	DispHeight = h;
	DispBytesPerPixel = bpp;
}

Void DispInit(UIntPtr fb) {
	for (UIntPtr i = 0; i < DispWidth * DispHeight * DispBytesPerPixel; i += MM_PAGE_SIZE) {						// Let's map the frame buffer to the virtual memory!
		if (!MmMap(DispFrameBuffer + i, fb + i, MM_MAP_KDEF)) {
			DbgWriteFormated("PANIC! Couldn't init the display\r\n");
			while (1) ;
		}
	}
	
	StrSetMemory((PVoid)DispFrameBuffer, 0, DispWidth * DispHeight * DispBytesPerPixel);							// Clear the screen
}
