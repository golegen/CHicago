// File author is Ítalo Lima Marconato Matias
//
// Created on July 21 of 2018, at 01:25 BRT
// Last edited on October 26 of 2018, at 22:56 BRT

#include <chicago/arch/bootmgr.h>

#include <chicago/display.h>

Void BootmgrDisplayPreInit(Void) {
	DispPreInit(BootmgrDispWidth, BootmgrDispHeight, BootmgrDispBpp / 8);
}

Void BootmgrDisplayInit(Void) {
	DispInit(BootmgrDispPhysAddr);
}
