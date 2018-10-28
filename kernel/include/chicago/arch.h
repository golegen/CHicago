// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:15 BRT
// Last edited on October 27 of 2018, at 22:14 BRT

#ifndef __CHICAGO_ARCH_H__
#define __CHICAGO_ARCH_H__

#include <chicago/types.h>

Void ArchInit(Void);
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
