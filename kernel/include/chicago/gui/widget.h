// File author is Ítalo Lima Marconato Matias
//
// Created on October 14 of 2018, at 15:50 BRT
// Last edited on October 14 of 2018, at 18:33 BRT

#ifndef __CHICAGO_GUI_WIDGET_H__
#define __CHICAGO_GUI_WIDGET_H__

#include <chicago/gui/surface.h>

typedef struct GuiWidgetStruct {
	GuiSurface surface;
	Void (*free)(struct GuiWidgetStruct*);
	Void (*draw)(struct GuiWidgetStruct*, PGuiSurface);
} GuiWidget, *PGuiWidget;

#endif		// __CHICAGO_WIDGET_H__
