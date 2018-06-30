// File author is Ítalo Lima Marconato Matias
//
// Created on June 30 of 2018, at 10:39 BRT
// Last edited on June 30 of 2018, at 10:40 BRT

#ifndef __CHICAGO_DISPLAY_H__
#define __CHICAGO_DISPLAY_H__

#include <chicago/types.h>

#define DISP_COLOR_BLACK 0
#define DISP_COLOR_BLUE 1
#define DISP_COLOR_GREEN 2
#define DISP_COLOR_CYAN 3
#define DISP_COLOR_RED 4
#define DISP_COLOR_MAGENTA 5
#define DISP_COLOR_BROWN 6
#define DISP_COLOR_LIGHT_GRAY 7
#define DISP_COLOR_DARK_GRAY 8
#define DISP_COLOR_LIGHT_BLUE 9
#define DISP_COLOR_LIGHT_GREEN 10
#define DISP_COLOR_LIGHT_CYAN 11
#define DISP_COLOR_LIGHT_RED 12
#define DISP_COLOR_LIGHT_MAGENTA 13
#define DISP_COLOR_YELLOW 14
#define DISP_COLOR_WHITE 15

UInt8 DispGetCursorX(Void);
UInt8 DispGetCursorY(Void);
Boolean DispGetCursorHide(Void);
Void DispSetCursorX(UInt8 x);
Void DispSetCursorY(UInt8 y);
Void DispSetCursorHide(Boolean hide);
Void DispSetCursor(UInt8 x, UInt8 y, Boolean hide);
UInt8 DispGetBackground(Void);
UInt8 DispGetForeground(Void);
Void DispSetBackground(UInt8 color);
Void DispSetForeground(UInt8 color);
Void DispSetColor(UInt8 back, UInt8 fore);
Void DispClearScreen(Void);
Void DispWriteCharacter(Char data);
Void DispWriteString(Const PChar data);
Void DispWriteInteger(UIntPtr data, UInt8 base);
Void DispWriteFormated(Const PChar data, ...);
Boolean DispPreInit(Void);
Boolean DispInit(Void);

#endif		// __CHICAGO_DISPLAY_H__

