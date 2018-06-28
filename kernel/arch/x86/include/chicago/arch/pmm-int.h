// File author is Ítalo Lima Marconato Matias
//
// Created on May 31 of 2018, at 18:46 BRT
// Last edited on June 28 of 2018, at 18:56 BRT

#ifndef __CHICAGO_ARCH_PMM_INT_H__
#define __CHICAGO_ARCH_PMM_INT_H__

#include <chicago/types.h>

extern UIntPtr MmBootAllocPointer;
extern PUIntPtr MmPageStack;
extern IntPtr MmPageStackPointer;
extern UIntPtr MmMaxPages;
extern UIntPtr MmUsedPages;

extern UIntPtr KernelStart;
extern UIntPtr KernelEnd;

#ifndef __CHICAGO_PMM__
extern UIntPtr KernelRealEnd;
#endif

Void PMMInit(Void);

#endif		// __CHICAGO_ARCH_PMM_INT_H__
