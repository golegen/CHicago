// File author is Ítalo Lima Marconato Matias
//
// Created on July 27 of 2018, at 14:42 BRT
// Last edited on November 16 of 2018, at 01:38 BRT

#ifndef __CHICAGO_PROCESS_H__
#define __CHICAGO_PROCESS_H__

#include <chicago/arch/process.h>
#include <chicago/alloc-int.h>
#include <chicago/queue.h>

#define PsLockTaskSwitch(i) Boolean i ## e = PsTaskSwitchEnabled; if (i ## e) PsTaskSwitchEnabled = False;
#define PsUnlockTaskSwitch(i) if (i ## e) PsTaskSwitchEnabled = True;
#define PsDontRequeue ((PVoid)1)

typedef Boolean Lock, *PLock;

typedef struct {
	UIntPtr id;
	PChar name;
	UIntPtr dir;
	PContext ctx;
	PAllocBlock alloc_base;
	UIntPtr mem_usage;
	PList handle_list;
	PList global_handle_list;
	PList files;
	IntPtr last_fid;
} Process, *PProcess;

#ifndef __CHICAGO_PROCESS__
extern Boolean PsTaskSwitchEnabled;
extern PProcess PsCurrentProcess;
extern PQueue PsProcessQueue;
extern PList PsProcessList;
#endif

PProcess PsCreateProcessInt(PChar name, UIntPtr entry, UIntPtr dir);
PProcess PsCreateProcess(PChar name, UIntPtr entry);
Void PsAddProcess(PProcess proc);
PProcess PsGetProcess(UIntPtr id);
Void PsExitProcess(Void);
Void PsSleep(UIntPtr ms);
Void PsWaitProcess(UIntPtr pid);
Void PsLock(PLock lock);
Void PsUnlock(PLock lock);
PContext PsCreateContext(UIntPtr entry);
Void PsFreeContext(PContext context);
Void PsSwitchTask(PVoid priv);
Void PsInit(Void);

#endif		// __CHICAGO_PROCESS_H__
