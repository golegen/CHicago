// File author is Ítalo Lima Marconato Matias
//
// Created on May 31 of 2018, at 18:47 BRT
// Last edited on June 22 of 2018, at 19:07 BRT

#ifndef __CHICAGO_ARCH_PMM_H__
#define __CHICAGO_ARCH_PMM_H__

#include <chicago/types.h>

UInt32 PMMDumbAlloc(UInt32 size, Boolean align);
UInt32 PMMAllocFrame(Void);
Void PMMFreeFrame(UInt32 frame);
Void PMMInit(Void);

#endif		// __CHICAGO_ARCH_PMM_H__
