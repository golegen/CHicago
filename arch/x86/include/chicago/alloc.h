// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:45 BRT
// Last edited on January 19 of 2019, at 14:02 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

UIntPtr MemAllocate(UIntPtr size);
UIntPtr MemAAllocate(UIntPtr size, UIntPtr align);;
Void MemFree(UIntPtr block);
Void MemAFree(UIntPtr block);
UIntPtr MemZAllocate(UIntPtr size);

#endif		// __CHICAGO_ALLOC_H__
