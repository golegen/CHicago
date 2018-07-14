// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 19:26 BRT
// Last edited on July 13 of 2018, at 18:51 BRT

#ifndef __CHICAGO_ARCH_VMM_H__
#define __CHICAGO_ARCH_VMM_H__

#include <chicago/types.h>

#define MmGetPDE(pd, i) ((PUInt32)(pd))[((i) & 0xFFFFF000) >> 22]
#define MmSetPDE(pd, i, p, f) ((PUInt32)(pd))[((i) & 0xFFFFF000) >> 22] = ((p) & 0xFFFFF000) | ((f) & 0xFFF)

#define MmGetPTE(pt, i) ((PUInt32)(pt))[((i) & 0xFFFFF000) >> 12]
#define MmSetPTE(pt, i, p, f) ((PUInt32)(pt))[((i) & 0xFFFFF000) >> 12] = ((p) & 0xFFFFF000) | ((f) & 0xFFF)

#define MmGetPTEInt(pt, i) ((PUInt32)(pt))[(((i) & 0xFFFFF000) >> 12) & 0x3FFF]
#define MmSetPTEInt(pt, i, p, f) ((PUInt32)(pt))[(((i) & 0xFFFFF000) >> 12) & 0x3FFF] = ((p) & 0xFFFFF000) | ((f) & 0xFFF)

#ifndef __CHICAGO_VMM__
extern PUInt32 MmCurrentDirectory;
extern PUInt32 MmCurrentTables;
extern PUInt32 MmTempAddresses;
#endif

#endif		// __CHICAGO_ARCH_VMM_H__
