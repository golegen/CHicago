// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 15:20 BRT
// Last edited on October 23 of 2018, at 16:09 BRT

#include <chicago/console.h>
#include <chicago/string.h>

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
