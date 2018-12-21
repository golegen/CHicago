// File author is Ítalo Lima Marconato Matias
//
// Created on December 19 of 2018, at 23:26 BRT
// Last edited on December 20 of 2018, at 18:18 BRT

#include <chicago/subarch.h>
#include <chicago/console.h>
#include <chicago/file.h>

Void ArchHalt(Void) {
	while (True) {							// Disable interrupts and halt
		Asm Volatile("cli; hlt");
	}
}

Void LoaderMain(Void) {
	ConClearScreen();						// Clear the screen
	SubarchInit();							// Init the subarchitecture-dependent bits of the loader
	FsInit();								// Init the filesystem
	ArchHalt();								// Halt
}
