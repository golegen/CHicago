// File author is Ítalo Lima Marconato Matias
//
// Created on May 27 of 2018, at 10:38 BRT
// Last edited on May 27 of 2018, at 11:48 BRT

#ifndef __CHICAGO_ARCH_REGISTERS_H__
#define __CHICAGO_ARCH_REGISTERS_H__

#include <chicago/types.h>

typedef struct
{
	UInt64 gs, fs, es, ds;
	UInt64 rdi, rsi, rbp, rbx, rdx, rcx, rax;
	UInt64 int_num, err_code;
	UInt64 rip, cs, rflags, userrsp, ss;
} Registers, *PRegisters;

typedef Void (*PInterruptHandler)(PRegisters);

#endif		// __CHICAGO_ARCH_REGISTERS_H__
