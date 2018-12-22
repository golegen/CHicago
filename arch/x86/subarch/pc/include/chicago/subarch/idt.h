// File author is Ítalo Lima Marconato Matias
//
// Created on May 26 of 2018, at 23:21 BRT
// Last edited on December 20 of 2018, at 22:30 BRT

#ifndef __CHICAGO_SUBARCH_IDT_H__
#define __CHICAGO_SUBARCH_IDT_H__

#include <chicago/subarch/registers.h>

Void IDTSetGate(UInt8 num, UInt32 base, UInt16 selector, UInt8 type);
Void IDTRegisterInterruptHandler(UInt8 num, PInterruptHandler handler);
Void IDTRegisterIRQHandler(UInt8 num, PInterruptHandler handler);
Void IDTUnregisterInterruptHandler(UInt8 num);
Void IDTUnregisterIRQHandler(UInt8 num);
Void IDTInit(Void);

#endif		// __CHICAGO_SUBARCH_IDT_H__
