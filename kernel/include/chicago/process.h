// File author is Ítalo Lima Marconato Matias
//
// Created on July 27 of 2018, at 14:42 BRT
// Last edited on October 19 of 2018, at 21:45 BRT

#ifndef __CHICAGO_PROCESS_H__
#define __CHICAGO_PROCESS_H__

#include <chicago/alloc-int.h>
#include <chicago/file.h>
#include <chicago/list.h>

#define PsCurrentProcess (PsCurrentThread->parent)

#define PsTMapBack(i, v, f, b) if (i ## d != MmKernelDirectory) { UIntPtr phys = MmGetPhys((UIntPtr)v); MmSwitchDirectory(i ## d); if (MmMap((UIntPtr)v, phys, f)) { b } MmSwitchDirectory(MmKernelDirectory); }
#define PsFMapBack(i, v, f, b) if (i ## d != MmKernelDirectory) { UIntPtr phys = MmGetPhys((UIntPtr)v); MmSwitchDirectory(i ## d); if (!MmMap((UIntPtr)v, phys, f)) { b } MmSwitchDirectory(MmKernelDirectory); }
#define PsLockTaskSwitch(i) Boolean i ## e = PsTaskSwitchEnabled; UIntPtr i ## d = MmGetCurrentDirectory(); if (i ## e) PsTaskSwitchEnabled = False; if (i ## d != MmKernelDirectory) MmSwitchDirectory(MmKernelDirectory)
#define PsUnlockTaskSwitch(i) if (i ## e) PsTaskSwitchEnabled = True; if (i ## d != MmKernelDirectory) MmSwitchDirectory(i ## d)

#define PS_TIMESLICE 20

struct ThreadStruct;
struct ProcessStruct;
struct ProcessDataStruct;
struct ThreadDataStruct;

typedef Volatile Boolean Lock, *PLock;

typedef struct ThreadStruct {
	UIntPtr id;
	PVoid priv;
	UIntPtr time;
	UIntPtr retv;
	struct ThreadStruct *next;
	struct ThreadStruct *prev;
	struct ThreadStruct *waitt;
	struct ProcessStruct *waitp;
	struct ProcessStruct *parent;
	struct ThreadDataStruct *thdata;
} Thread, *PThread;

typedef struct ProcessStruct {
	UIntPtr id;
	PChar name;
	UIntPtr dir;
	UIntPtr lasttid;
	Int lastfid;
	PList threads;
	PList files;
	PAllocBlock alloc_base;
	UIntPtr mem_usage;
} Process, *PProcess;

typedef struct ThreadDataStruct {
	UIntPtr errno;
} ThreadData, *PThreadData;

typedef struct {
	PFsNode file;
	Int num;
} ProcessFile, *PProcessFile;

#ifndef __CHICAGO_PROCESS__
extern Boolean PsTaskSwitchEnabled;
extern PThread PsCurrentThread;
extern PList PsWaittList;
extern PList PsWaitpList;
#endif

PThread PsCreateThreadInt(PProcess proc, UIntPtr entry);
PProcess PsCreateProcessInt(PChar name, UIntPtr entry, UIntPtr dir);
PThread PsCreateThread(UIntPtr entry);
PProcess PsCreateProcess(PChar name, UIntPtr entry);
Void PsAddThread(PThread th);
Void PsAddProcess(PProcess proc);
PThread PsGetThread(UIntPtr tid);
PProcess PsGetProcess(UIntPtr pid);
Void PsSleep(UIntPtr ms);
UIntPtr PsWaitThread(PThread th);
UIntPtr PsWaitProcess(PProcess proc);
Void PsLock(PLock lock);
Void PsUnlock(PLock lock);
Void PsWakeup(PList list, PThread th);
Void PsExitThread(UIntPtr ret);
Void PsExitProcess(UIntPtr ret);
PVoid PsCreateThreadPrivateData(UIntPtr entry);
Void PsFreeThreadPrivateData(PVoid priv);
Void PsSwitchTask(PVoid priv);
Void PsInitInt(Void);
Void PsInit(Void);

#endif		// __CHICAGO_PROCESS_H__
