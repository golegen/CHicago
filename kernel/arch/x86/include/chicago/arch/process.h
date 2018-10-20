// File author is Ítalo Lima Marconato Matias
//
// Created on July 28 of 2018, at 01:15 BRT
// Last edited on October 19 of 2018, at 21:43 BRT

#ifndef __CHICAGO_ARCH_PROCESS_H__
#define __CHICAGO_ARCH_PROCESS_H__

#include <chicago/types.h>

#define PS_STACK_SIZE 1024

typedef struct {
	UInt8 kstack[PS_STACK_SIZE];
	UInt8 fpu_state[512];
	UIntPtr esp;
} ThreadPrivateData, *PThreadPrivateData;

#ifndef __CHICAGO_ARCH_PROCESS__
extern UInt8 PsFPUStateSave[512];
extern UInt8 PsFPUDefaultState[512];
#endif

UIntPtr PsGetEIP(Void);
UIntPtr PsGetEAX(Void);

#endif		// __CHICAGO_ARCH_PROCESS_H__
