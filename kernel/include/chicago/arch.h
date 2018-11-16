// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:15 BRT
// Last edited on November 16 of 2018, at 01:42 BRT

#ifndef __CHICAGO_ARCH_H__
#define __CHICAGO_ARCH_H__

#include <chicago/types.h>

Void ArchHalt(Void);
Void ArchInit(Void);
Void ArchInitSc(Void);
Void ArchInitFPU(Void);
Void ArchInitPMM(Void);
Void ArchInitVMM(Void);
Void ArchInitDebug(Void);
Void ArchInitMouse(Void);
Void ArchInitDisplay(Void);
Void ArchInitKeyboard(Void);
UIntPtr ArchGetSeconds(Void);
Void ArchPanic(UInt32 err, PVoid priv);

#endif		// __CHICAGO_ARCH_H__
