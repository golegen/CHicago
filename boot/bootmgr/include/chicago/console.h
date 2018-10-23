// File author is Ítalo Lima Marconato Matias
//
// Created on October 23 of 2018, at 15:57 BRT
// Last edited on October 23 of 2018, at 16:10 BRT

#ifndef __CHICAGO_CONSOLE_H__
#define __CHICAGO_CONSOLE_H__

#include <chicago/types.h>

#define CON_COLOR_NORMAL 0x00
#define CON_COLOR_INVERSE 0x01
#define CON_COLOR_WARNING 0x02
#define CON_COLOR_ERROR 0x03
#define CON_COLOR_PANIC 0x04

Void ConHideCursor(Void);
Void ConShowCursor(Void);
Void ConSetColor(UInt8 c);
Void ConClearScreen(Void);
Void ConWriteCharacter(Char data);
Void ConWriteString(PChar data);
Void ConWriteInteger(UIntPtr data, UInt8 base);
Void ConWriteFormated(PChar data, ...);

#endif		// __CHICAGO_CONSOLE_H__
