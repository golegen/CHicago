// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:17 BRT
// Last edited on July 20 of 2018, at 18:27 BRT

#ifndef __CHICAGO_DISPLAY_H__
#define __CHICAGO_DISPLAY_H__

UIntPtr DispGetFrameBuffer(Void);
UIntPtr DispGetWidth(Void);
UIntPtr DispGetHeight(Void);
UIntPtr DispGetBytesPerPixel(Void);
PUIntPtr DispGetPalette256(Void);
UIntPtr DispMatchColors(UInt8 ar, UInt8 ag, UInt8 ab, UInt8 br, UInt8 bg, UInt8 bb);
Void DispExtractRGB(UIntPtr c, PUInt8 r, PUInt8 g, PUInt8 b);
UInt8 DispConvertTo256(UIntPtr c);
Void DispPreInit(UIntPtr w, UIntPtr h, UIntPtr bpp);
Void DispInit(UIntPtr fb);

#endif		// __CHICAGO_DISPLAY_H__
