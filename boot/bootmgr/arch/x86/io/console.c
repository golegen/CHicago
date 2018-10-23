// File author is Ítalo Lima Marconato Matias
//
// Created on October 23 of 2018, at 15:45 BRT
// Last edited on October 23 of 2018, at 16:14 BRT

#include <chicago/arch/port.h>

#include <chicago/console.h>
#include <chicago/string.h>

PUInt16 ConBuffer = (PUInt16)0xB8000;
Boolean ConCursorVisible = True;
UInt8 ConColor = 0x07;
UInt8 ConCursorX = 0;
UInt8 ConCursorY = 0;

static Void ConMoveCursor(Void) {
	if (!ConCursorVisible) {													// The cursor is invisible?
		PortOutWord(0x3D4, 0x200A);												// Yes, let's hide it
		PortOutWord(0x3D4, 0x0B);
		return;
	}
	
	UInt16 pos = ConCursorY * 80 + ConCursorX;									// No, so let's set the cursor position to x, y
	
	PortOutByte(0x3D4, 0x0E);
	PortOutByte(0x3D5, pos >> 8);
	PortOutByte(0x3D4, 0x0F);
	PortOutByte(0x3D5, pos);
}

Void ConHideCursor(Void) {
	ConCursorVisible = False;													// Make the cursor invisible
}

Void ConShowCursor(Void) {
	ConCursorVisible = True;													// Make the cursor visible
}

Void ConSetColor(UInt8 c) {
	if (c == CON_COLOR_NORMAL) {												// Black background, light gray foreground
		ConColor = 0x07;
	} else if (c == CON_COLOR_INVERSE) {										// Light gray background, black foreground
		ConColor = 0x70;
	} else if (c == CON_COLOR_WARNING) {										// Black background, yellow foreground
		ConColor = 0x0E;
	} else if (c == CON_COLOR_ERROR) {											// Black background, red foreground
		ConColor = 0x04;
	} else if (c == CON_COLOR_PANIC) {											// Red background, white foreground
		ConColor = 0x4F;
	}
}

Void ConClearScreen(Void) {
	StrSetMemory16(ConBuffer, ' ' | (ConColor << 8), 2000);						// Clear the screen
	ConCursorX = ConCursorY = 0;												// Set the cursor x and y
	ConMoveCursor();															// Move the cursor
}

Void ConWriteCharacter(Char data) {
	switch (data) {
	case '\b': {																// Backspace
		if (ConCursorX > 0) {													// We have any more characters to delete in this line?
			ConCursorX++;														// Yes
		} else if (ConCursorY > 0) {											// We have any more lines?																										
			ConCursorY--;														// Yes
			ConCursorX = 79;
		}
		
		break;
	}
	case '\n': {																// Line feed
		ConCursorY++;
		break;
	}
	case '\r': {																// Carriage return
		ConCursorX = 0;
		break;
	}
	case '\t': {																// Tab
		ConCursorX = (ConCursorX + 4) & ~3;
		break;
	}
	default: {																	// Character
		ConBuffer[ConCursorY * 80 + ConCursorX++] = data | (ConColor << 8);
		break;
	}
	}
	
	if (ConCursorX >= 80) {														// Go to the next line?
		ConCursorX = 0;															// Yes
		ConCursorY++;
	}
	
	if (ConCursorY >= 25) {														// Scroll up?
		StrCopyMemory(ConBuffer, ConBuffer + 80, 1920);							// Yes
		StrSetMemory(ConBuffer + 1920, ' ' | (ConColor << 8), 80);
		ConCursorY = 24;
	}
	
	ConMoveCursor();
}
