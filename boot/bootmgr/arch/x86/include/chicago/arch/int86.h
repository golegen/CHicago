// File author is Ítalo Lima Marconato Matias
//
// Created on October 26 of 2018, at 18:45 BRT
// Last edited on October 26 of 2018, at 18:47 BRT

#ifndef __CHICAGO_ARCH_INT86_H__
#define __CHICAGO_ARCH_INT86_H__

#include <chicago/types.h>

typedef struct
{
	UInt16 di, si, bp, sp, bx, dx, cx, ax;
} Int86Registers, *PInt86Registers;

Void Int86(UInt32 num, UInt32 stack);

#endif		// __CHICAGO_ARCH_INT86_H__
