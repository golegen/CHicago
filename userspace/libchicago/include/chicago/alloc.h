// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 18:00 BRT
// Last edited on November 15 of 2018, at 18:02 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

UIntPtr MmAllocMemory(UIntPtr size);
Void MmFreeMemory(UIntPtr block);
UIntPtr MmReallocMemory(UIntPtr block, UIntPtr size);
UIntPtr MmGetUseage(Void);

#endif		// __CHICAGO_ALLOC_H__
