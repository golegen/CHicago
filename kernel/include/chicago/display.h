// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:17 BRT
// Last edited on July 19 of 2018, at 02:23 BRT

#ifndef __CHICAGO_DISPLAY_H__
#define __CHICAGO_DISPLAY_H__

UIntPtr DispGetFrameBuffer(Void);
UIntPtr DispGetWidth(Void);
UIntPtr DispGetHeight(Void);
UIntPtr DispGetBytesPerPixel(Void);
Void DispPreInit(UIntPtr w, UIntPtr h, UIntPtr bpp);
Void DispInit(UIntPtr fb);

#endif		// __CHICAGO_DISPLAY_H__
