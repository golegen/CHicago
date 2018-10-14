// File author is Ítalo Lima Marconato Matias
//
// Created on October 14 of 2018, at 15:51 BRT
// Last edited on October 14 of 2018, at 18:33 BRT

#ifndef __CHICAGO_GUI_WINDOW_H__
#define __CHICAGO_GUI_WINDOW_H__

#include <chicago/gui/widget.h>
#include <chicago/list.h>

typedef struct {
	List widgets;
	GuiSurface tsurface;
	GuiSurface usurface;
} GuiWindow, *PGuiWindow;

PGuiWindow GuiCreateWindow(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h);
Void GuiFreeWindow(PGuiWindow window);
Void GuiAddWidget(PGuiWindow window, PGuiWidget widget);
Void GuiRemoveWidget(PGuiWindow window, PGuiWidget widget);

#endif		// __CHICAGO_WINDOW_H__
