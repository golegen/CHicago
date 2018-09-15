// File author is Ítalo Lima Marconato Matias
//
// Created on July 21 of 2018, at 01:25 BRT
// Last edited on September 14 of 2018, at 15:59 BRT

#include <chicago/arch/multiboot.h>

#include <chicago/display.h>

Boolean MultibootDisplayPreInit(Void) {
	PMultibootVbeInfo vbeinfo = (PMultibootVbeInfo)(MultibootHeaderPointer->vbe_mode_info + 0xC0000000);
	
	if (vbeinfo != Null) {																																		// Mode was set by bootloader?
		DispPreInit(vbeinfo->width, vbeinfo->height, vbeinfo->bpp / 8);																							// Yes :)
		return True;
	} else {
		return False;																																			// ...
	}
}

Void MultibootDisplayInit(Void) {
	DispInit(((PMultibootVbeInfo)(MultibootHeaderPointer->vbe_mode_info))->phys);
}
