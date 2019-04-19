// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 14:01 BRT
// Last edited on April 19 of 2019, at 17:44 BRT

#ifndef __CHICAGO_ARCH_H__
#define __CHICAGO_ARCH_H__

#include <chicago/types.h>

Void ArchJumpInt(UIntPtr dest, PChar bootdev, UIntPtr mmap, UIntPtr mmapc, UIntPtr data, UIntPtr options);
IntPtr ArchJump(UIntPtr dest, PChar bootdev, UIntPtr options);
Void ArchInit(Void);
Void ArchHalt(Void);

#endif		// __CHICAGO_ARCH_H__
