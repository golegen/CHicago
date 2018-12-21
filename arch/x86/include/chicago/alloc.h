// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:45 BRT
// Last edited on December 20 of 2018, at 12:17 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

UIntPtr MemAllocate(UIntPtr size);
Void MemFree(UIntPtr block);
UIntPtr MemZAllocate(UIntPtr size);

#endif		// __CHICAGO_ALLOC_H__
