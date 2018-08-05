// File author is Ítalo Lima Marconato Matias
//
// Created on July 21 of 2018, at 01:25 BRT
// Last edited on July 25 of 2018, at 14:15 BRT

#include <chicago/arch/multiboot.h>

#include <chicago/display.h>
#include <chicago/mm.h>

Boolean MultibootDisplayPreInit(Void) {
	if (MultibootHeaderPointer->flags & (1 << 12)) {																											// Mode was set by bootloader?
		DispPreInit(MultibootHeaderPointer->framebuffer_width, MultibootHeaderPointer->framebuffer_height, MultibootHeaderPointer->framebuffer_bpp / 8);		// Yes :)
		return True;
	} else {
		return False;																																			// ...
	}
}

Void MultibootDisplayInit(Void) {
	if (MultibootHeaderPointer->framebuffer_address_low < 0xE0000000) {																							// *HACHHACHHACH*
		DispInit(0xE8000000);
	} else {
		DispInit(MultibootHeaderPointer->framebuffer_address_low);
	}
}
