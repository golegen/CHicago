// File author is Ítalo Lima Marconato Matias
//
// Created on October 22 of 2018, at 18:36 BRT
// Last edited on October 23 of 2018, at 16:13 BRT

#include <chicago/console.h>
#include <chicago/version.h>

Void LoaderMain(Void) {
	ConClearScreen();															// Clear the screen
	ConWriteFormated("CHicago Boot Manager for %s\r\n", CHICAGO_ARCH);			// Print some information
	ConWriteFormated("Codename '%s'\r\n", CHICAGO_CODENAME);
	ConWriteFormated("%s\r\n", CHICAGO_VSTR);
	
	while (1) ;
}
