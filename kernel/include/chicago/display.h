// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:17 BRT
// Last edited on July 21 of 2018, at 14:11 BRT

#ifndef __CHICAGO_DISPLAY_H__
#define __CHICAGO_DISPLAY_H__

UIntPtr DispGetFrameBuffer(Void);
UIntPtr DispGetWidth(Void);
UIntPtr DispGetHeight(Void);
UIntPtr DispGetBytesPerPixel(Void);
Void DispExtractRGB(UIntPtr c, PUInt8 r, PUInt8 g, PUInt8 b);
Void DispPutPixel(UIntPtr x, UIntPtr y, UIntPtr c);
Void DispDrawLine(UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c);
Void DispDrawRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void DispFillRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void DispDrawCharacter(UIntPtr x, UIntPtr y, UIntPtr bg, UIntPtr fg, Char data);
Void DispDrawString(UIntPtr x, UIntPtr y, UIntPtr bg, UIntPtr fg, PChar data);
Void DispIncrementProgessBar(Void);
Void DispFillProgressBar(Void);
Void DispDrawProgessBar(Void);
Void DispPreInit(UIntPtr w, UIntPtr h, UIntPtr bpp);
Void DispInit(UIntPtr fb);

#endif		// __CHICAGO_DISPLAY_H__
