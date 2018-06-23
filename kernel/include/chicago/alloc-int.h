// File author is Ítalo Lima Marconato Matias
//
// Created on June 17 of 2018, at 21:42 BRT
// Last edited on June 17 of 2018, at 21:42 BRT

#ifndef __CHICAGO_ALLOC_INT_H__
#define __CHICAGO_ALLOC_INT_H__

#include <chicago/types.h>

typedef struct MemoryAllocBlockStruct {
	UInt32 size;
	UInt32 free;
	UInt32 addr;
	struct MemoryAllocBlockStruct *next;
	struct MemoryAllocBlockStruct *prev;
	Char data[1];
} MemoryAllocBlock, *PMemoryAllocBlock;

#endif		// __CHICAGO_ALLOC_INT_H__
