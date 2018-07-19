// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:12 BRT
// Last edited on July 19 of 2018, at 02:22 BRT

#include <chicago/debug.h>
#include <chicago/mm.h>

UIntPtr DispFrameBuffer = 0;
UIntPtr DispWidth = 0;
UIntPtr DispHeight = 0;
UIntPtr DispBytesPerPixel = 0;

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

Void DispPreInit(UIntPtr w, UIntPtr h, UIntPtr bpp) {
	DispFrameBuffer = MmBootAlloc(w * h * bpp, False);						// Alloc some virt space for the frame buffer
	DispWidth = w;
	DispHeight = h;
	DispBytesPerPixel = bpp;
}

Void DispInit(UIntPtr fb) {
	if (!MmMap(DispFrameBuffer, fb, MM_MAP_KDEF)) {							// Try to map it!
		DbgWriteFormated("PANIC! Couldn't init the display\r\n");
		while (1) ;
	}
}
