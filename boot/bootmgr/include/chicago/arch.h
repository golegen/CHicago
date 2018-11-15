// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 14:01 BRT
// Last edited on November 15 of 2018, at 15:58 BRT

#ifndef __CHICAGO_ARCH_H__
#define __CHICAGO_ARCH_H__

#include <chicago/types.h>

IntPtr ArchJump(UIntPtr dest, PChar bootdev);
Void ArchHalt(Void);
Void ArchInit(Void);

#endif		// __CHICAGO_ARCH_H__

