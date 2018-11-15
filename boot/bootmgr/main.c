// File author is Ítalo Lima Marconato Matias
//
// Created on October 22 of 2018, at 18:36 BRT
// Last edited on November 15 of 2018, at 15:49 BRT

#include <chicago/arch.h>
#include <chicago/config.h>
#include <chicago/console.h>
#include <chicago/file.h>
#include <chicago/menu.h>

Void LoaderMain(Void) {
	ConClearScreen();				// Clear the screen
	ArchInit();						// Init the architecture-dependent bits of the loader
	FsInit();						// Init the filesystem
	ConfInit();						// Read the config file
	MenuInit();						// Init the menu
	MenuLoop();						// Enter the menu loop
	ArchHalt();						// Halt
}
