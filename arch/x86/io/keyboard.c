// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 15:17 BRT
// Last edited on January 21 of 2019, at 12:10 BRT

#include <efi.h>
#include <efilib.h>

#include <chicago/types.h>

UInt8 KbdReadKey(Void) {
	EFI_INPUT_KEY key;
	
	while (ST->ConIn->ReadKeyStroke(ST->ConIn, &key) == EFI_NOT_READY) ;																						// Read from the keyboard
	
	if ((key.UnicodeChar == '\r') || (key.UnicodeChar == '\n')) {																								// Enter?
		return 0x1C;																																			// Yes
	} else if (key.ScanCode == 0x01) {																															// Up?
		return 0x48;																																			// Yes
	} else if (key.ScanCode == 0x02) {																															// Down?
		return 0x50;																																			// Yes
	}
	
	return 0;
}
