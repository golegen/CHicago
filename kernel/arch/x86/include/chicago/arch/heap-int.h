// File author is Ítalo Lima Marconato Matias
//
// Created on June 04 of 2018, at 15:54 BRT
// Last edited on June 04 of 2018, at 21:26 BRT

#ifndef __CHICAGO_ARCH_HEAP_INT_H__
#define __CHICAGO_ARCH_HEAP_INT_H__

#include <chicago/types.h>

typedef struct {
	UInt32 current_position_aligned;
} HeapPrivateData, *PHeapPrivateData;

#endif		// __CHICAGO_ARCH_HEAP_INT_H__
