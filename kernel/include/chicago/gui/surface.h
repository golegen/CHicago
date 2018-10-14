// File author is Ítalo Lima Marconato Matias
//
// Created on October 14 of 2018, at 15:48 BRT
// Last edited on October 14 of 2018, at 18:25 BRT

#ifndef __CHICAGO_GUI_SURFACE_H__
#define __CHICAGO_GUI_SURFACE_H__

#include <chicago/types.h>

typedef struct {
	UIntPtr x;
	UIntPtr y;
	UIntPtr width;
	UIntPtr height;
} GuiSurface, *PGuiSurface;

PGuiSurface GuiCreateSurface(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h);
Void GuiPutPixel(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr c);
Void GuiDrawLine(PGuiSurface surface, UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c);
Void GuiDrawRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void GuiFillRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void GuiDrawRoundedRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);
Void GuiFillRoundedRectangle(PGuiSurface surface, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);

#endif		// __CHICAGO_GUI_SURFACE_H__
