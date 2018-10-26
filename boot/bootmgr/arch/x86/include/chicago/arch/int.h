// File author is Ítalo Lima Marconato Matias
//
// Created on October 26 of 2018, at 18:45 BRT
// Last edited on October 26 of 2018, at 20:15 BRT

#ifndef __CHICAGO_ARCH_INT_H__
#define __CHICAGO_ARCH_INT_H__

#include <chicago/types.h>

typedef struct {
	UInt32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
} Int86Registers, *PInt86Registers;

typedef struct {
	UInt32 base_low;
	UInt32 base_high;
	UInt32 length_low;
	UInt32 length_high;
	UInt32 type;
	UInt32 acpi;
} Packed SMAPEntry, *PSMAPEntry;

Void Int86(UInt32 num, UInt32 stack);

#endif		// __CHICAGO_ARCH_INT_H__
