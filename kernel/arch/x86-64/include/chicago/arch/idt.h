// File author is Ítalo Lima Marconato Matias
//
// Created on May 27 of 2018, at 10:35 BRT
// Last edited on May 27 of 2018, at 10:35 BRT

#ifndef __CHICAGO_ARCH_IDT_H__
#define __CHICAGO_ARCH_IDT_H__

#include <chicago/types.h>

Void IDTSetGate(UInt8 num, UInt64 base, UInt16 selector, UInt8 type);
Void IDTInit(Void);

#endif		// __CHICAGO_ARCH_IDT_H__
