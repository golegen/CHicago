// File author is Ítalo Lima Marconato Matias
//
// Created on June 04 of 2018, at 15:52 BRT
// Last edited on June 04 of 2018, at 15:53 BRT

#ifndef __CHICAGO_HEAP_H__
#define __CHICAGO_HEAP_H__

#include <chicago/types.h>

typedef struct {
	UInt32 signature;		// 0x0CAFEEC1
	PChar name;
	Boolean is_kernel;
	UInt32 start;
	UInt32 current_position;
	PVoid private_data;
} Heap, *PHeap;

#ifndef __CHICAGO_HEAP__
extern PHeap KernelHeap;
#endif

PHeap HeapCreate(PChar name, UInt32 start);
Void HeapDelete(PHeap heap);
PChar HeapGetName(PHeap heap);
UInt32 HeapGetStart(PHeap heap);
UInt32 HeapGetCurrent(PHeap heap);
Boolean HeapIncrement(PHeap heap, UInt32 amount);
Boolean HeapDecrement(PHeap heap, UInt32 amount);

#endif		// __CHICAGO_HEAP_H__
