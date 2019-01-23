// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 14:01 BRT
// Last edited on January 23 of 2019, at 14:50 BRT

#ifndef __CHICAGO_ARCH_H__
#define __CHICAGO_ARCH_H__

#include <chicago/types.h>

Void ArchJumpInt(UIntPtr dest, PChar bootdev, UIntPtr mmap, UIntPtr mmapc, UIntPtr data);
IntPtr ArchJump(UIntPtr dest, PChar bootdev);
Void ArchInit(Void);
Void ArchHalt(Void);

#endif		// __CHICAGO_ARCH_H__
