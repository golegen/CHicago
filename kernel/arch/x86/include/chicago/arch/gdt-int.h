// File author is Ítalo Lima Marconato Matias
//
// Created on May 26 of 2018, at 21:19 BRT
// Last edited on May 26 of 2018, at 21:19 BRT

#ifndef __CHICAGO_ARCH_GDT_INT_H__
#define __CHICAGO_ARCH_GDT_INT_H__

#include <chicago/types.h>

extern Void GDTLoad(UInt32 base, UInt16 limit);

#endif		// __CHICAGO_ARCH_GDT_INT_H__
