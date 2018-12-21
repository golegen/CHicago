// File author is Ítalo Lima Marconato Matias
//
// Created on December 20 of 2018, at 11:41 BRT
// Last edited on December 20 of 2018, at 11:50 BRT

#include <efi.h>
#include <efilib.h>

#include <chicago/console.h>

Void ConHideCursor(Void) {
	ST->ConOut->EnableCursor(ST->ConOut, FALSE);						// Redirect to EnableCursor
}

Void ConShowCursor(Void) {
	ST->ConOut->EnableCursor(ST->ConOut, TRUE);							// Redirect to EnableCursor
}

Void ConSetColor(UInt8 c) {
	if (c == CON_COLOR_NORMAL) {										// Black background, light gray foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x07);						// Redirect to SetAttribute
	} else if (c == CON_COLOR_INVERSE) {								// Light gray background, black foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x70);						// Redirect to SetAttribute
	} else if (c == CON_COLOR_WARNING) {								// Black background, yellow foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x0E);						// Redirect to SetAttribute
	} else if (c == CON_COLOR_ERROR) {									// Black background, red foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x04);						// Redirect to SetAttribute
	} else if (c == CON_COLOR_PANIC) {									// Red background, white foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x4F);						// Redirect to SetAttribute
	}
}

Void ConClearScreen(Void) {
	ST->ConOut->ClearScreen(ST->ConOut);								// Redirect to ClearScreen
}

Void ConWriteCharacter(Char data) {
	UInt16 wstr[2] = { data, L'\0' };									// Create our wide string
	ST->ConOut->OutputString(ST->ConOut, wstr);							// And redirect to OutputString
}
