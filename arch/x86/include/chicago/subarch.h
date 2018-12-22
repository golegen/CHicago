// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 14:01 BRT
// Last edited on December 21 of 2018, at 23:23 BRT

#ifndef __CHICAGO_SUBARCH_H__
#define __CHICAGO_SUBARCH_H__

#include <chicago/types.h>

Void SubarchJumpInt(UInt32 dest, PChar bootdev, UInt32 mmap, UInt32 mmapc, PUInt32 data);
IntPtr SubarchJump(UIntPtr dest, PChar bootdev);
Void SubarchInit(Void);
Void ArchHalt(Void);

#endif		// __CHICAGO_SUBARCH_H__
