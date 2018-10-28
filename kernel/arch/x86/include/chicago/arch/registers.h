// File author is Ítalo Lima Marconato Matias
//
// Created on May 26 of 2018, at 23:18 BRT
// Last edited on October 27 of 2018, at 22:35 BRT

#ifndef __CHICAGO_ARCH_REGISTERS_H__
#define __CHICAGO_ARCH_REGISTERS_H__

#include <chicago/types.h>

typedef struct
{
	UInt32 gs, fs, es, ds;
	UInt32 edi, esi, ebp, ebx, edx, ecx, eax;
	UInt32 int_num, err_code;
	UInt32 eip, cs, eflags, esp, ss;
} Registers, *PRegisters;

typedef Void (*PInterruptHandler)(PRegisters);

#endif		// __CHICAGO_ARCH_REGISTERS_H__
