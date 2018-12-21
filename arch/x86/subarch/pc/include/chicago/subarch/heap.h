// File author is Ítalo Lima Marconato Matias
//
// Created on June 29 of 2018, at 22:31 BRT
// Last edited on December 20 of 2018, at 12:25 BRT

#ifndef __CHICAGO_SUBARCH_HEAP_H__
#define __CHICAGO_SUBARCH_HEAP_H__

#include <chicago/types.h>

UIntPtr HeapGetCurrent(Void);
UIntPtr HeapGetStart(Void);
UIntPtr HeapGetEnd(Void);
Boolean HeapIncrement(UIntPtr amount);
Boolean HeapDecrement(UIntPtr amount);

#endif		// __CHICAGO_SUBARCH_HEAP_H__
