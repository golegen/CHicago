// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 14:01 BRT
// Last edited on December 21 of 2018, at 11:56 BRT

#ifndef __CHICAGO_SUBARCH_H__
#define __CHICAGO_SUBARCH_H__

#include <chicago/types.h>

Void SubarchJumpInt(UIntPtr dest, PChar bootdev, UIntPtr mmap, UIntPtr mmapc, UIntPtr data);
IntPtr SubarchJump(UIntPtr dest, PChar bootdev);
Void SubarchInit(Void);
Void ArchHalt(Void);

#endif		// __CHICAGO_SUBARCH_H__
