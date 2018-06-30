// File author is Ítalo Lima Marconato Matias
//
// Created on June 30 of 2018, at 10:36 BRT
// Last edited on June 30 of 2018, at 10:40 BRT

#include <chicago/arch/port.h>

#include <chicago/display.h>
#include <chicago/mm.h>

PUInt16 DispFrameBuffer = 0;
Boolean DispCursorHide = False;
UInt8 DispCursorX = 0;
UInt8 DispCursorY = 0;
UInt16 DispColor = 0;
UInt8 DispHeight = 0;
UInt8 DispWidth = 0;

Void DispUpdateCursor(Void) {
	if (!DispCursorHide) {
		UInt16 pos = (DispCursorY * DispWidth) + DispCursorX;														// Get the real cursor position
		
		PortOutByte(0x3D4, 0x0F);																					// Send the cursor low byte
		PortOutByte(0x3D5, (UInt8)(pos & 0xFF));
		PortOutByte(0x3D4, 0x0E);																					// And the high byte
		PortOutByte(0x3D5, (UInt8)((pos >> 8) & 0xFF));
	}
}

Void DispScrollScreen(Void) {
	if (DispCursorY >= DispHeight) {																				// Only scroll if the cursor is out of the screen
		for (UInt16 i = 0; i < DispHeight; i++) {																	// Move one line up!
			for (UInt16 j = 0; j < DispWidth; j++) {
				DispFrameBuffer[(i * DispWidth) + j] = DispFrameBuffer[((i + 1) * DispWidth) + j];
			}
		}
		
		DispCursorY = DispHeight - 1;																				// Set y to the last line
		
		DispUpdateCursor();																							// Update the cursor
	}
}

UInt8 DispGetCursorX(Void) {
	return DispCursorX;
}

UInt8 DispGetCursorY(Void) {
	return DispCursorY;
}

Boolean DispGetCursorHide(Void) {
	return DispCursorHide;
}

Void DispSetCursorX(UInt8 x) {
	if (x >= DispWidth) {
		x = DispWidth - 1;
	}
	
	DispCursorX = x;
	
	DispUpdateCursor();
}

Void DispSetCursorY(UInt8 y) {
	if (y >= DispHeight) {
		y = DispHeight - 1;
	}
	
	DispCursorY = y;
	
	DispUpdateCursor();
}

Void DispSetCursorHide(Boolean hide) {
	DispCursorHide = hide;
	
	if (hide) {
		PortOutWord(0x3D4, 0x200A);
		PortOutWord(0x3D4, 0x0B);
	} else {
		PortOutWord(0x3D4, 0xE0A);
		PortOutWord(0x3D4, 0xF0B);
	}
}

Void DispSetCursor(UInt8 x, UInt8 y, Boolean hide) {
	DispSetCursorX(x);
	DispSetCursorY(y);
	DispSetCursorHide(hide);
}

UInt8 DispGetBackground(Void) {
	return (DispColor & 0xF0) >> 4;
}

UInt8 DispGetForeground(Void) {
	return DispColor & 0x0F;
}

Void DispSetBackground(UInt8 color) {
	DispColor = ((color & 0xF0) << 4) | (DispGetForeground() & 0x0F);
}

Void DispSetForeground(UInt8 color) {
	DispColor = ((DispGetBackground() & 0x0F) << 4) | (color & 0x0F);
}

Void DispSetColor(UInt8 back, UInt8 fore) {
	DispSetBackground(back);
	DispSetForeground(fore);
}

Void DispClearScreen(Void) {
	for (UInt16 i = 0; i < DispWidth * DispHeight; i++) {
		DispFrameBuffer[i] = (DispColor << 8) | ' ';																// Clear the screen with spaces using the current color
	}
	
	DispCursorX = DispCursorY = 0;																					// Set the cursor x and y to 0
	
	DispUpdateCursor();																								// And update the cursor
}

Void DispWriteCharacter(Char data) {
	switch (data) {
	case '\b': {																									// Backspace?
		if (DispCursorY > 0) {																						// Yes, are we in the line 0?
			if (DispCursorX > 0) {																					// No, x > 0?
				DispCursorX--;																						// Yes, so just decrement x
			} else {
				DispCursorY--;																						// No, so decrement y
				DispCursorX = DispWidth - 1;																		// And set x to width-1
			}
		} else if (DispCursorX > 0) {
			DispCursorX--;																							// Yes, so only decrement x (if it's greater than 0)
		}
		
		break;
	}
	case '\n': {																									// New line?
		DispCursorY++;																								// Yes, increment y
		DispCursorX = 0;																							// And set x to zero
		break;
	}
	case '\r': {																									// Carriage return?
		DispCursorX = 0;																							// Yes, set x to zero
		break;
	}
	case '\t': {																									// Tab?
		DispCursorX = ((DispCursorX + 4) / 4) * 4;																	// Yes
		break;
	}
	default: {
		DispFrameBuffer[(DispCursorY * DispWidth) + DispCursorX] = (DispColor << 8) | data;
		DispCursorX++;
	}
	}
	
	if (DispCursorX >= DispWidth) {																					// X is too high?
		DispCursorY++;																								// Yes, so increment y
		DispCursorX = 0;																							// And set x to zero
	}
	
	DispScrollScreen();																								// Scroll
	DispUpdateCursor();																								// And update the cursor
}

Void DispWriteString(Const PChar data) {
	for (UInt32 i = 0; data[i] != '\0'; i++) {
		DispWriteCharacter(data[i]);
	}
}

Void DispWriteInteger(UIntPtr data, UInt8 base) {
	if (data == 0) {
		DispWriteCharacter('0');
		return;
	}
	
	static Char buf[32] = { 0 };
	Int i = 30;
	
	for (; data && i; i--, data /= base) {
		buf[i] = "0123456789ABCDEF"[data % base];
	}
	
	DispWriteString(&buf[i + 1]);
}

Void DispWriteFormated(Const PChar data, ...) {
	VariadicList va;
	VariadicStart(va, data);																						// Let's start our va list with the arguments provided by the user (if any)
	
	for (UInt32 i = 0; data[i] != '\0'; i++) {
		if (data[i] != '%') {																						// It's an % (integer, string, character or other)?
			DispWriteCharacter(data[i]);																			// Nope
		} else {
			switch (data[++i]) {																					// Yes! So let's parse it!
			case 's': {																								// String
				DispWriteString((PChar)VariadicArg(va, PChar));
				break;
			}
			case 'c': {																								// Character
				DispWriteCharacter((Char)VariadicArg(va, Int));
				break;
			}
			case 'd': {																								// Decimal Number
				DispWriteInteger((UIntPtr)VariadicArg(va, UIntPtr), 10);
				break;
			}
			case 'x': {																								// Hexadecimal Number
				DispWriteInteger((UIntPtr)VariadicArg(va, UIntPtr), 16);
				break;
			}
			default: {																								// None of the others...
				DispWriteCharacter(data[i]);
				break;
			}
			}
		}
	}
	
	VariadicEnd(va);
}

Boolean DispPreInit(Void) {
	DispFrameBuffer = (PUInt16)MmBootAlloc(0x1000, True);															// Alloc some space
	DispWidth = 80;																									// Default width of text mode is 80
	DispHeight = 25;																								// And the default height is 25
	
	if (!MmMap((UIntPtr)DispFrameBuffer, 0xB8000, MM_MAP_KDEF)) {													// Map in the current temp page directory
		return False;																								// Failed...
	}
	
	DispSetCursor(0, 0, False);																						// Set the cursor pos to 0, 0 and make it visible
	DispSetColor(DISP_COLOR_BLACK, DISP_COLOR_LIGHT_GRAY);															// Set the color
	DispClearScreen();																								// Clear the screen
	
	return True;
}

Boolean DispInit(Void) {
	if (!MmMap((UIntPtr)DispFrameBuffer, 0xB8000, MM_MAP_KDEF)) {													// Map to the current page directory
		return False;																								// Failed...
	} else {
		return True;
	}
}
