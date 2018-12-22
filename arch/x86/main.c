// File author is Ítalo Lima Marconato Matias
//
// Created on December 19 of 2018, at 23:26 BRT
// Last edited on December 21 of 2018, at 22:59 BRT

#include <chicago/config.h>
#include <chicago/console.h>
#include <chicago/file.h>
#include <chicago/menu.h>
#include <chicago/subarch.h>

Void ArchHalt(Void) {
	while (True) {							// Disable interrupts and halt
		Asm Volatile("cli; hlt");
	}
}

Void LoaderMain(Void) {
	ConClearScreen();						// Clear the screen
	SubarchInit();							// Init the subarchitecture-dependent bits of the loader
	FsInit();								// Init the filesystem
	ConfInit();								// Read the config file
	MenuInit();								// Init the menu
	MenuLoop();								// Enter the menu loop
	ArchHalt();								// Halt
}
