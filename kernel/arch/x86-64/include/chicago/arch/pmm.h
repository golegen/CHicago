// File author is Ítalo Lima Marconato Matias
//
// Created on July 12 of 2018, at 21:25 BRT
// Last edited on July 13 of 2018, at 12:38 BRT

#ifndef __CHICAGO_ARCH_PMM_H__
#define __CHICAGO_ARCH_PMM_H__

#include <chicago/types.h>

extern UIntPtr MmBootAllocPointer;
extern PUIntPtr MmPageStack;
extern PUIntPtr MmPageReferences;
extern IntPtr MmPageStackPointer;
extern UIntPtr MmMaxPages;
extern UIntPtr MmUsedPages;

extern UIntPtr KernelStart;
extern UIntPtr KernelEnd;

#ifndef __CHICAGO_PMM__
extern UIntPtr KernelRealEnd;
#endif

Void PMMInit(Void);

#endif		// __CHICAGO_ARCH_PMM_H__
