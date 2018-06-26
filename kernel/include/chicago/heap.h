// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:14 BRT
// Last edited on June 26 of 2018, at 19:24 BRT

#ifndef __CHICAGO_HEAP_H__
#define __CHICAGO_HEAP_H__

#include <chicago/types.h>

UInt32 HeapGetStart(Void);
UInt32 HeapGetCurrent(Void);
Boolean HeapIncrement(UInt32 amount);
Boolean HeapDecrement(UInt32 amount);
Void HeapInit(Void);

#endif		// __CHICAGO_HEAP_H__
