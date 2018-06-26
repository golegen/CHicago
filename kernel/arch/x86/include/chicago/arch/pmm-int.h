// File author is Ítalo Lima Marconato Matias
//
// Created on May 31 of 2018, at 18:46 BRT
// Last edited on June 23 of 2018, at 20:39 BRT

#ifndef __CHICAGO_ARCH_PMM_INT_H__
#define __CHICAGO_ARCH_PMM_INT_H__

#include <chicago/types.h>

#define SET_BIT(m, i) (m)[(i) / 32] |= (1 << ((i) % 32))
#define CLEAR_BIT(m, i) (m)[(i) / 32] &= ~(1 << ((i) % 32))
#define TEST_BIT(m, i) ((m)[(i) / 32] & (1 << ((i) % 32)))

extern UInt32 PMMDumbAllocPointer;

extern UInt32 KernelStart;
extern UInt32 KernelEnd;

#ifndef __CHICAGO_PMM__
extern UInt32 KernelRealEnd;
#endif

#endif		// __CHICAGO_ARCH_PMM_INT_H__
