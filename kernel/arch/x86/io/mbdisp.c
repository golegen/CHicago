// File author is Ítalo Lima Marconato Matias
//
// Created on July 21 of 2018, at 01:25 BRT
// Last edited on July 21 of 2018, at 01:25 BRT

#include <chicago/arch/multiboot.h>

#include <chicago/display.h>

Boolean MultibootDisplayPreInit(Void) {
	if (MultibootHeaderPointer->flags & (1 << 12)) {																											// Mode was set by bootloader?
		DispPreInit(MultibootHeaderPointer->framebuffer_width, MultibootHeaderPointer->framebuffer_height, MultibootHeaderPointer->framebuffer_bpp / 8);		// Yes :)
		return True;
	} else {
		return False;																																			// ... We're going to need to use the... V G A
	}
}

Void MultibootDisplayInit(Void) {
	DispInit(MultibootHeaderPointer->framebuffer_address_low);
}
