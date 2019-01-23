// File author is Ítalo Lima Marconato Matias
//
// Created on December 19 of 2018, at 23:26 BRT
// Last edited on January 23 of 2019, at 14:56 BRT

#include <chicago/arch.h>
#include <chicago/config.h>
#include <chicago/console.h>
#include <chicago/file.h>
#include <chicago/menu.h>

Void ArchHalt(Void) {
	while (True) {							// Disable interrupts and halt
		Asm Volatile("cli; hlt");
	}
}

Void LoaderMain(Void) {
	ConClearScreen();						// Clear the screen
	ArchInit();								// Init some x86-only stuff
	FsInit();								// Init the filesystem
	ConfInit();								// Read the config file
	MenuInit();								// Init the menu
	MenuLoop();								// Enter the menu loop
	ArchHalt();								// Halt
}
