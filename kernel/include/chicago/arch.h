// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:15 BRT
// Last edited on October 19 of 2018, at 14:20 BRT

#ifndef __CHICAGO_ARCH_H__
#define __CHICAGO_ARCH_H__

#include <chicago/types.h>

Void ArchInit(Void);
Void ArchInitFPU(Void);
Void ArchInitPMM(Void);
Void ArchInitDebug(Void);
Void ArchInitMouse(Void);
Void ArchInitDisplay(Void);
Void ArchInitKeyboard(Void);
UIntPtr ArchGetSeconds(Void);
Void ArchPreInitDisplay(Void);

#endif		// __CHICAGO_ARCH_H__
