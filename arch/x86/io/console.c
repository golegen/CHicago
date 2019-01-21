// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 15:20 BRT
// Last edited on January 21 of 2019, at 12:10 BRT

#define StrDuplicate Dummy

#include <efi.h>
#include <efilib.h>

#undef StrDuplicate

#include <chicago/console.h>
#include <chicago/string.h>

Void ConHideCursor(Void) {
	ST->ConOut->EnableCursor(ST->ConOut, FALSE);								// Redirect to EnableCursor
}

Void ConShowCursor(Void) {
	ST->ConOut->EnableCursor(ST->ConOut, TRUE);									// Redirect to EnableCursor
}

Void ConSetColor(UInt8 c) {
	if (c == CON_COLOR_NORMAL) {												// Black background, light gray foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x07);								// Redirect to SetAttribute
	} else if (c == CON_COLOR_INVERSE) {										// Light gray background, black foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x70);								// Redirect to SetAttribute
	} else if (c == CON_COLOR_WARNING) {										// Black background, yellow foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x0E);								// Redirect to SetAttribute
	} else if (c == CON_COLOR_ERROR) {											// Black background, red foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x04);								// Redirect to SetAttribute
	} else if (c == CON_COLOR_PANIC) {											// Red background, white foreground
		ST->ConOut->SetAttribute(ST->ConOut, 0x4F);								// Redirect to SetAttribute
	}
}

Void ConClearScreen(Void) {
	ST->ConOut->ClearScreen(ST->ConOut);										// Redirect to ClearScreen
}

Void ConWriteCharacter(Char data) {
	UInt16 wstr[2] = { data, L'\0' };											// Create our wide string
	ST->ConOut->OutputString(ST->ConOut, wstr);									// And redirect to OutputString
}

Void ConWriteString(PChar data) {
	if (data == Null) {
		return;
	}
	
	for (UIntPtr i = 0; i < StrGetLength(data); i++) {							// Write all the characters from the string
		ConWriteCharacter(data[i]);
	}
}

Void ConWriteInteger(UIntPtr data, UInt8 base) {
	if (data == 0) {
		ConWriteCharacter('0');
		return;
	}
	
	static Char buf[32] = { 0 };
	Int i = 30;
	
	for (; data && i; i--, data /= base) {
		buf[i] = "0123456789ABCDEF"[data % base];
	}
	
	ConWriteString(&buf[i + 1]);
}

Void ConWriteFormated(PChar data, ...) {
	if (data == Null) {
		return;
	}
	
	VariadicList va;
	VariadicStart(va, data);													// Let's start our va list with the arguments provided by the user (if any)
	
	for (UIntPtr i = 0; i < StrGetLength(data); i++) {
		if (data[i] != '%') {													// It's an % (integer, string, character or other)?
			ConWriteCharacter(data[i]);											// No
		} else {
			switch (data[++i]) {												// Yes, let's parse it!
			case 's': {															// String
				ConWriteString((PChar)VariadicArg(va, PChar));
				break;
			}
			case 'c': {															// Character
				ConWriteCharacter((Char)VariadicArg(va, Int));
				break;
			}
			case 'd': {															// Decimal Number
				ConWriteInteger((UIntPtr)VariadicArg(va, UIntPtr), 10);
				break;
			}
			case 'x': {															// Hexadecimal Number
				ConWriteInteger((UIntPtr)VariadicArg(va, UIntPtr), 16);
				break;
			}
			default: {															// Probably it's another % (probably)
				ConWriteCharacter(data[i]);
				break;
			}
			}
		}
	}
	
	VariadicEnd(va);
}
