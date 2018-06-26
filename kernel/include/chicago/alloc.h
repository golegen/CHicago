// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:24 BRT
// Last edited on June 26 of 2018, at 19:24 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

UInt32 MemoryAlloc(UInt32 size);
UInt32 MemoryAlignedAlloc(UInt32 size, UInt32 align);
Void MemoryFree(UInt32 blockk);
Void MemoryAlignedFree(UInt32 block);
UInt32 MemoryZAlloc(UInt32 size);
UInt32 MemoryRealloc(UInt32 blockk, UInt32 size);

#endif		// __CHICAGO_ALLOC_H__

