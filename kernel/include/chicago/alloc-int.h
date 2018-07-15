// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:45 BRT
// Last edited on July 15 of 2018, at 02:38 BRT

#ifndef __CHICAGO_ALLOC_INT_H__
#define __CHICAGO_ALLOC_INT_H__

#include <chicago/types.h>

typedef struct MemAllocateBlockStruct {
	UIntPtr size;
	UInt32 free;
	UIntPtr addr;
	struct MemAllocateBlockStruct *next;
	struct MemAllocateBlockStruct *prev;
	Char data[1];
} MemAllocateBlock, *PMemAllocateBlock;

#endif		// __CHICAGO_ALLOC_INT_H__
