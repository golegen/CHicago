// File author is Ítalo Lima Marconato Matias
//
// Created on October 14 of 2018, at 15:46 BRT
// Last edited on October 14 of 2018, at 15:51 BRT

#ifndef __CHICAGO_GUI_H__
#define __CHICAGO_GUI_H__

#include <chicago/gui/window.h>

typedef struct {
	PGuiWindow window;
	UIntPtr dir;
} GuiWindowInt, *PGuiWindowInt;

Void GuiAddWindow(PGuiWindow window, Boolean topmost);
Void GuiRemoveWindow(PGuiWindow window);
Void GuiRefresh(Void);
Void GuiInit(Void);

#endif		// __CHICAGO_GUI_H__
