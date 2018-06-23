// File author is Ítalo Lima Marconato Matias
//
// Created on May 12 of 2018, at 20:57 BRT
// Last edited on May 13 of 2018, at 11:23 BRT

#ifndef __CHICAGO_ARCH_GDT_H__
#define __CHICAGO_ARCH_GDT_H__

#include <chicago/types.h>

Void GDTSetGate(UInt8 num, UInt32 base, UInt32 limit, UInt8 type, UInt8 gran);
Void GDTInit(Void);

#endif		// __CHICAGO_ARCH_GDT_H__
