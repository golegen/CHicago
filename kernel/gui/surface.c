// File author is Ítalo Lima Marconato Matias
//
// Created on October 14 of 2018, at 18:26 BRT
// Last edited on October 17 of 2018, at 21:26 BRT

#include <chicago/display.h>
#include <chicago/gui/surface.h>
#include <chicago/mm.h>
#include <chicago/string.h>

PGuiSurface GuiCreateSurface(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h) {
	if (w == 0 || h == 0) {																												// We can't have 0-sized surfaces!
		return Null;
	}
	
	PGuiSurface surface = (PGuiSurface)MmAllocUserMemory(sizeof(GuiSurface));															// Alloc the memory for the struct
	
	if (surface != Null) {																												// Failed?
		surface->x = x;																													// No, so let's fill the data!
		surface->y = y;
		surface->width = w;
		surface->height = h;
	}
	
	return surface;
}

Void GuiPutPixel(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr c) {
	if (surface == Null || ((UIntPtr)surface) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	if (x >= surface->width) {																											// Fix the x and the y if they are bigger than the surface dimensions
		x = surface->width - 1;
	}
	
	if (y >= surface->height) {
		y = surface->height - 1;
	}
	
	DispPutPixel(surface->x + x, surface->y + y, c);																					// Use the DispPutPixel function
}

Void GuiDrawLine(PGuiSurface surface, UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c) {
	if (surface == Null || ((UIntPtr)surface) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	if (x0 >= surface->width) {																											// Fix the x and the y if they are bigger than the surface dimensions
		x0 = surface->width - 1;
	}
	
	if (y0 >= surface->height) {
		y0 = surface->height - 1;
	}
	
	if (x1 >= surface->width) {
		x1 = surface->width - 1;
	}
	
	if (y1 >= surface->height) {
		y1 = surface->height - 1;
	}
	
	DispDrawLine(surface->x + x0, surface->y + y0, surface->x + x1, surface->y + y1, c);												// Use the DispDrawLine function
}

Void GuiDrawRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c) {
	if (surface == Null || ((UIntPtr)surface) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	if (x >= surface->width) {																											// Fix the x and the y if they are bigger than the surface dimensions
		x = surface->width - 1;
	}
	
	if (y >= surface->height) {
		y = surface->height - 1;
	}
	
	if ((x + w) > surface->width) {																										// Fix the width and the height if they are bigger than the surface dimensions
		w = surface->width - x;
	}
	
	if ((y + h) > surface->height) {
		h = surface->height - y;
	}
	
	DispDrawRectangle(surface->x + x, surface->y + y, w, h, c);																			// Use the DispDrawRectangle function
}

Void GuiFillRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c) {
	if (surface == Null || ((UIntPtr)surface) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	if (x >= surface->width) {																											// Fix the x and the y if they are bigger than the surface dimensions
		x = surface->width - 1;
	}
	
	if (y >= surface->height) {
		y = surface->height - 1;
	}
	
	if ((x + w) > surface->width) {																										// Fix the width and the height if they are bigger than the surface dimensions
		w = surface->width - x;
	}
	
	if ((y + h) > surface->height) {
		h = surface->height - y;
	}
	
	DispFillRectangle(surface->x + x, surface->y + y, w, h, c);																			// Use the DispFillRectangle function
}

Void GuiDrawRoundedRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c) {
	if (surface == Null || ((UIntPtr)surface) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	if (x >= surface->width) {																											// Fix the x and the y if they are bigger than the surface dimensions
		x = surface->width - 1;
	}
	
	if (y >= surface->height) {
		y = surface->height - 1;
	}
	
	if ((x + w) > surface->width) {																										// Fix the width and the height if they are bigger than the surface dimensions
		w = surface->width - x;
	}
	
	if ((y + h) > surface->height) {
		h = surface->height - y;
	}
	
	DispDrawRoundedRectangle(surface->x + x, surface->y + y, w, h, r, c);																// Use the DispDrawRoundedRectangle function
}

Void GuiFillRoundedRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c) {
	if (surface == Null || ((UIntPtr)surface) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	if (x >= surface->width) {																											// Fix the x and the y if they are bigger than the surface dimensions
		x = surface->width - 1;
	}
	
	if (y >= surface->height) {
		y = surface->height - 1;
	}
	
	if ((x + w) > surface->width) {																										// Fix the width and the height if they are bigger than the surface dimensions
		w = surface->width - x;
	}
	
	if ((y + h) > surface->height) {
		h = surface->height - y;
	}
	
	DispFillRoundedRectangle(surface->x + x, surface->y + y, w, h, r, c);																// Use the DispFillRoundedRectangle function
}

Void GuiWriteCharacter(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr c, IntPtr scale, Char data) {
	if (scale == 0 || surface == Null || ((UIntPtr)surface) >= MM_USER_END) {
		return;
	}
	
	for (IntPtr i = 0; i < (16 * scale); i++) {
		for (IntPtr j = (8 * scale) - 1; j >= 0; j--) {
			UIntPtr rx = j * 8 / (8 * scale);																							// Calculate the real x
			UIntPtr ry = i * 16 / (16 * scale);																							// And the real y in the character bitmap
			
			if (DispFont[(UInt8)data][ry] & (1 << rx)) {																				// And put it to the screen (if we need)
				GuiPutPixel(surface, x + j, y + i, c);
			}
		}
	}
}

Void GuiWriteString(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr c, IntPtr scale, PChar data) {
	if (scale == 0 || surface == Null/* || data == Null*/ || ((UIntPtr)surface) >= MM_USER_END/* || ((UIntPtr)data) >= MM_USER_END*/) {	// Valid pointer? Valid scale?
		return;																															// No >:(
	}
	
	for (UIntPtr i = 0; i < StrGetLength(data); i++) {
		GuiWriteCharacter(surface, x + (i * (8 * scale)), y, c, scale, data[i]);														// Just write all the characters in this string
	}
}
