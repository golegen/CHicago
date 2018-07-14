// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:45 BRT
// Last edited on July 14 of 2018, at 00:34 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

UIntPtr MemoryAlloc(UIntPtr size);
UIntPtr MemoryAlignedAlloc(UIntPtr size, UIntPtr align);
Void MemoryFree(UIntPtr blockk);
Void MemoryAlignedFree(UIntPtr block);
UIntPtr MemoryZAlloc(UIntPtr size);
UIntPtr MemoryAlignedZAlloc(UIntPtr size, UIntPtr align);
UIntPtr MemoryRealloc(UIntPtr blockk, UIntPtr size);
UIntPtr MemoryAlignedRealloc(UIntPtr blockkk, UIntPtr size, UIntPtr align);

#endif		// __CHICAGO_ALLOC_H__
