// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:45 BRT
// Last edited on July 13 of 2018, at 00:45 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

UInt32 MemoryAlloc(UInt32 size);
UInt32 MemoryAlignedAlloc(UInt32 size, UInt32 align);
Void MemoryFree(UInt32 blockk);
Void MemoryAlignedFree(UInt32 block);
UInt32 MemoryZAlloc(UInt32 size);
UInt32 MemoryAlignedZAlloc(UInt32 size, UInt32 align);
UInt32 MemoryRealloc(UInt32 blockk, UInt32 size);
UInt32 MemoryAlignedRealloc(UInt32 blockkk, UInt32 size, UInt32 align);

#endif		// __CHICAGO_ALLOC_H__
