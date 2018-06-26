// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:15 BRT
// Last edited on June 26 of 2018, at 19:15 BRT

#ifndef __CHICAGO_ALLOC_INT_H__
#define __CHICAGO_ALLOC_INT_H__

#include <chicago/types.h>

typedef struct MemoryAllocBlockStruct {
	UInt32 size;
	UInt32 free;
	UInt32 addr;
	struct MemoryAllocBlockStruct *next;
	struct MemoryAllocBlockStruct *prev;
	Char data[0];
} MemoryAllocBlock, *PMemoryAllocBlock;

#endif		// __CHICAGO_ALLOC_INT_H__
