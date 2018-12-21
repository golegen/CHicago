// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:45 BRT
// Last edited on December 20 of 2018, at 12:25 BRT

#ifndef __CHICAGO_SUBARCH_ALLOC_INT_H__
#define __CHICAGO_SUBARCH_ALLOC_INT_H__

#include <chicago/types.h>

typedef struct AllocBlockStruct {
	UIntPtr size;
	UIntPtr start;
	Boolean free;
	struct AllocBlockStruct *next;
	struct AllocBlockStruct *prev;
} AllocBlock, *PAllocBlock;

#endif		// __CHICAGO_SUBARCH_ALLOC_INT_H__
