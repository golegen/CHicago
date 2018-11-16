// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 18:04 BRT
// Last edited on November 16 of 2018, at 16:34 BRT

#ifndef __CHICAGO_PROCESS_H__
#define __CHICAGO_PROCESS_H__

#include <chicago/types.h>

typedef Boolean Lock, *PLock;

UIntPtr PsCreateThread(UIntPtr entry);
UIntPtr PsGetTID(Void);
UIntPtr PsGetPID(Void);
Void PsSleep(UIntPtr ms);
Void PsWaitThread(UIntPtr tid);
Void PsWaitProcess(UIntPtr pid);
Void PsLock(PLock lock);
Void PsUnlock(PLock lock);
Void PsExitThread(Void);
Void PsExitProcess(Void);
Void PsForceSwitch(Void);

#endif		// __CHICAGO_PROCESS_H__
