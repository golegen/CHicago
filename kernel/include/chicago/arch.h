// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:15 BRT
// Last edited on October 15 of 2018, at 12:56 BRT

#ifndef __CHICAGO_ARCH_H__
#define __CHICAGO_ARCH_H__

#include <chicago/types.h>

Void ArchInitFPU(Void);
Void ArchInitPMM(Void);
Void ArchInitDebug(Void);
Void ArchInitDisplay(Void);
Void ArchInitKeyboard(Void);
UIntPtr ArchGetSeconds(Void);
Void ArchPreInitDisplay(Void);
Void ArchInit(Void);

#endif		// __CHICAGO_ARCH_H__
