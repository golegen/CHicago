// File author is Ítalo Lima Marconato Matias
//
// Created on July 27 of 2018, at 14:59 BRT
// Last edited on October 19 of 2018, at 21:44 BRT

#define __CHICAGO_PROCESS__

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/timer.h>
#include <chicago/virt.h>

extern Void KernelMainLate(Void);

Boolean PsTaskSwitchEnabled = False;
PThread PsCurrentThread = Null;
PList PsProcessList = Null;
PList PsWaittList = Null;
PList PsWaitpList = Null;
UIntPtr PsNextPID = 0;

PThread PsCreateThreadInt(PProcess proc, UIntPtr entry) {
	if (proc == Null) {																															// proc can't be Null! If you want to use the current process, don't use Null in this function, instead, use the PsCreateThread() function!
		return Null;
	}
	
	PThread th = (PThread)MemAllocate(sizeof(Thread));																							// Let's try to allocate the thread struct!
	
	if (th == Null) {
		return Null;																															// Failed
	}
	
	th->id = proc->lasttid++;																													// Use the next TID
	th->priv = PsCreateThreadPrivateData(entry);																								// Let's create our priv data
	th->time = PS_TIMESLICE - 1;																												// Set the default timeslice
	th->retv = 0;																																// This may be set in the PsExitThread/Process function
	th->next = th;																																// (This way it's easier to init the first process)
	th->prev = th;																																// ^
	th->waitt = Null;																															// This will be set in the PsWaitThread function
	th->waitp = Null;																															// This will be set in the PsWaitProcess function
	th->parent = proc;																															// Set our parent
	
	if (th->priv == Null) {
		MemFree((UIntPtr)th);																													// Failed to create the priv data
		return Null;
	}
	
	return th;
}

PProcess PsCreateProcessInt(PChar name, UIntPtr entry, UIntPtr dir) {
	PProcess proc = (PProcess)MemAllocate(sizeof(Process));																						// Let's try to allocate the process struct!
	
	if (proc == Null) {
		return Null;																															// Failed
	}
	
	proc->id = PsNextPID++;																														// Use the next PID
	proc->name = StrDuplicate(name);																											// Duplicate the name
	proc->dir = dir;																															// We're only going to create a new page directory if dir is Null
	proc->lasttid = 0;
	proc->lastfid = 0;
	proc->threads = ListNew(False, False);																										// Create the thread list
	
	if (proc->threads == Null) {
		MemFree((UIntPtr)proc->name);																											// Failed...
		MemFree((UIntPtr)proc);
		
		return Null;
	}
	
	proc->files = ListNew(False, False);																										// Create the file list
	
	if (proc->files == Null) {
		ListFree(proc->threads);																												// Failed...
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
	}
	
	proc->alloc_base = Null;
	proc->mem_usage = 0;
	
	PThread th = PsCreateThreadInt(proc, entry);																								// Let's create the first thread!
	
	if (th == Null) {																										
		ListFree(proc->files);																													// Failed...
		ListFree(proc->threads);
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		
		return Null;
	}
	
	if (!ListAdd(proc->threads, th)) {																											// Let's add it to this proc thread list!
		PsFreeThreadPrivateData(th->priv);																										// Failed...
		MemFree((UIntPtr)th);
		ListFree(proc->files);
		ListFree(proc->threads);
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		
		return Null;
	}
	
	if (proc->dir == 0) {																														// Create an new page dir?
		proc->dir = MmCreateDirectory();																										// Yes
		
		if (proc->dir == 0) {
			PsFreeThreadPrivateData(th->priv);																									// Failed...
			ListFree(proc->files);
			ListFree(proc->threads);
			MemFree((UIntPtr)proc->name);
			MemFree((UIntPtr)proc);
			
			return Null;
		}
	}
	
	UIntPtr old = MmGetCurrentDirectory();																										// Save our cur page dir
	
	if (old != proc->dir) {
		MmSwitchDirectory(proc->dir);																											// Switch to the new proc page dir
	}
	
	th->thdata = (PThreadData)VirtAllocAddress(0, sizeof(ThreadData), VIRT_FLAGS_HIGHEST | VIRT_PROT_READ | VIRT_PROT_WRITE);
	
	if (th->thdata == Null) {																													// Failed?
		if (old != MmGetCurrentDirectory()) {																									// Yes....
			MmSwitchDirectory(old);
		}
		
		MmFreeDirectory(proc->dir);
		PsFreeThreadPrivateData(th->priv);
		ListFree(proc->files);
		ListFree(proc->threads);
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		
		return Null;
	}
	
	th->thdata->errno = 0;
	
	if (old != MmGetCurrentDirectory()) {
		MmSwitchDirectory(old);																													// Switch back to the old page dir
	}
	
	return proc;
}

PThread PsCreateThread(UIntPtr entry) {
	if (PsCurrentThread == Null) {																												// This function requires the PsCurrentThread to be initialized
		return Null;
	}
	
	PsLockTaskSwitch(old);																														// LOCK!
	
	PThread th = PsCreateThreadInt(PsCurrentProcess, entry);																					// Use our PsCreateThreadInt function
	
	if (th == Null) {
		PsUnlockTaskSwitch(old);																												// Failed...
		return Null;
	}
	
	PsFMapBack(old, th, MM_MAP_KDEF, {																											// Map the thread struct into the parent page directory (oldd), as we're currently in the kernel directory
		PsFreeThreadPrivateData(th->priv);																										// Failed
		MemFree((UIntPtr)th);
		PsUnlockTaskSwitch(old);
		
		return Null;
	});
	
	PsFMapBack(old, th->priv, MM_MAP_KDEF, {																									// Map the priv
		PsFreeThreadPrivateData(th->priv);																										// Failed
		MemFree((UIntPtr)th);
		PsUnlockTaskSwitch(old);
		
		return Null;
	});
	
	PsUnlockTaskSwitch(old);																													// Unlock
	
	th->thdata = (PThreadData)VirtAllocAddress(0, sizeof(ThreadData), VIRT_FLAGS_HIGHEST | VIRT_PROT_READ | VIRT_PROT_WRITE);					// Alloc the thread data struct
	
	if (th->thdata == Null) {																													// Failed?
		PsLockTaskSwitch(old);																													// Yes...
		PsFreeThreadPrivateData(th->priv);
		MemFree((UIntPtr)th);
		PsUnlockTaskSwitch(old);
		
		return Null;
	}
	
	th->thdata->errno = 0;
	
	return th;
}

PProcess PsCreateProcess(PChar name, UIntPtr entry) {
	PsLockTaskSwitch(old);																														// Lock
	
	PProcess proc = PsCreateProcessInt(name, entry, 0);																							// Use our PsCreateProcessInt function
	
	if (proc == Null) {
		PsUnlockTaskSwitch(old);																												// Failed
		return Null;
	}
	
	PsUnlockTaskSwitch(old);																													// Unlock
	
	return proc;
}

Void PsAddThread(PThread th) {
	if ((th == Null) || (th->parent == Null) || (th->parent->threads == Null) || (PsCurrentThread == Null) || (PsCurrentProcess == Null)) {		// Sanity checks
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	UIntPtr idx = 0;																															// Let's search if this thread is already in the process thread list
	Boolean found = False;
	
	for (UIntPtr i = 0; i < !found && th->parent->threads->length; i++) {
		if (ListGet(th->parent->threads, idx) == th) {
			found = True;
		}
	}
	
	if (!found) {
		ListAdd(th->parent->threads, th);																										// Nope, add it
	}
	
	th->next = PsCurrentThread->next;																											// Add it to the thread queue
	th->prev = PsCurrentThread;
	PsCurrentThread->next = th;
	th->next->prev = th;
	
	PsUnlockTaskSwitch(old);																													// Unlock
}

Void PsAddProcess(PProcess proc) {
	if ((proc == Null) || (proc->dir == 0) || (proc->threads == Null) || (PsCurrentThread == Null) || (PsCurrentProcess == Null)) {				// Sanity checks
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	for (PListNode i = proc->threads->tail; ; i = i->prev) {																					// Let's add all the threads of this proc!
		PThread th = (PThread)i->data;
		
		th->next = PsCurrentThread->next;
		th->prev = PsCurrentThread;
		PsCurrentThread->next = th;
		th->next->prev = th;
		
		if (i == proc->threads->head) {																											// If we're in the last (first) entry, stop
			break;
		}
	}
	
	ListAdd(PsProcessList, proc);																												// Try to add the proc to the proc list
	PsUnlockTaskSwitch(old);																													// Unlock
}

PThread PsGetThread(UIntPtr tid) {
	if (PsCurrentThread == Null) {																												// Sanity checks
		return Null;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	ListForeach(PsCurrentProcess->threads, i) {																									// Let's search!
		PThread th = (PThread)i->data;
		
		if (th->id == tid) {																													// Match?
			PsUnlockTaskSwitch(old);																											// Yes :)
			return th;
		}
	}
	
	PsUnlockTaskSwitch(old);																													// ...
	
	return Null;
}

PProcess PsGetProcess(UIntPtr pid) {
	if (PsProcessList == Null) {																												// Sanity checks
		return Null;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	ListForeach(PsProcessList, i) {																												// Let's search!
		PProcess proc = (PProcess)i->data;
		
		if (proc->id == pid) {																													// Match?
			PsUnlockTaskSwitch(old);																											// Yes :)
			return proc;
		}
	}
	
	PsUnlockTaskSwitch(old);																													// ...
	
	return Null;
}

Void PsSleep(UIntPtr ms) {
	if (PsCurrentThread == Null) {																												// Thread list is initialized?
		TimerSleep(ms);																															// Nope, just TimerSleep
		return;
	} else if (PsCurrentThread == PsCurrentThread->next) {																						// We can add this thread to the sleep list?
		TimerSleep(ms);																															// Nope, just TimerSleep
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	if (ms == 0) {																																// ms == 0?
		PsUnlockTaskSwitch(old);																												// Yes, just PsSwitchTask
		PsSwitchTask(Null);
		return;
	}
	
	PsUnlockTaskSwitch(old);
	TimerSleepProcess(ms);																														// Use this function
}

UIntPtr PsWaitThread(PThread th) {
	if ((th == Null) || (PsWaittList == Null) || (PsCurrentThread == Null)) {																	// Sanity checks
		return 1;																																// ...
	} else if (PsCurrentThread == PsCurrentThread->next) {																						// We can add this thread to the wait list?
		return 1;																																// Nope
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	if (!ListAdd(PsWaittList, PsCurrentThread)) {																								// Try to add this thread to the wait list
		PsUnlockTaskSwitch(old);																												// Failed, but we're not going to give up!
		return PsWaitThread(th);
	}
	
	PsCurrentThread->waitt = th;
	
	while (PsCurrentThread->waitt != Null) {																									// Let's wait...
		PsSwitchTask(Null);
	}
	
	return PsCurrentThread->retv;
}

UIntPtr PsWaitProcess(PProcess proc) {
	if ((proc == Null) || (PsWaittList == Null) || (PsCurrentThread == Null)) {																	// Sanity checks
		return 1;																																// ...
	} else if (PsCurrentThread == PsCurrentThread->next) {																						// We can add this thread to the wait list?
		return 1;																																// Nope
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	if (!ListAdd(PsWaitpList, PsCurrentThread)) {																								// Try to add this thread to the wait list
		PsUnlockTaskSwitch(old);																												// Failed, but we're not going to give up
		return PsWaitProcess(proc);
	}
	
	PsCurrentThread->waitp = proc;
	
	while (PsCurrentThread->waitp != Null) {																									// Let's wait...
		PsSwitchTask(Null);
	}
	
	return PsCurrentThread->retv;
}

Void PsLock(PLock lock) {
	if ((lock == Null) || (PsCurrentThread == Null) || (PsCurrentThread == PsCurrentThread->next)) {											// Sanity checks
		return;
	}
	
	while (*lock) {																																// Just keep on checking!
		PsSwitchTask(Null);
	}
	
	*lock = True;
}

Void PsUnlock(PLock lock) {
	if ((lock == Null) || (PsCurrentThread == Null)) {																							// Sanity checks
		return;
	}
	
	*lock = False;																																// Unlock...
}

Void PsWakeup(PList list, PThread th) {
	if ((list == Null) || (th == Null) || (PsCurrentThread == Null)) {																			// Sanity checks...
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	UIntPtr idx = 0;																															// Let's try to find th in list
	Boolean found = False;
	
	ListForeach(list, i) {
		if (i->data == th) {																													// Found?
			found = True;																														// Yes!
			break;
		} else {
			idx++;																																// Nope
		}
	}
	
	if (!found) {																																// Found?
		PsUnlockTaskSwitch(old);																												// Nope
		return;
	}
	
	ListRemove(list, idx);																														// Remove th from list
	
	th->waitt = Null;
	th->waitp = Null;
	
	PsUnlockTaskSwitch(old);																													// Unlock
}

Void PsExitThread(UIntPtr ret) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null)) {																				// Sanity checks...
		return;
	} else if ((PsCurrentProcess->id == 0) && (PsCurrentThread->id == 0)) {																		// Kernel main thread?
		PsLockTaskSwitch(old);																													// ...
		DbgWriteFormated("PANIC! Tried to close the main kernel thread\r\n");
		while (1) ;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	if (PsCurrentProcess->threads->length == 1) {																								// This is the only thread of this process?
		PsUnlockTaskSwitch(old);																												// Yes, so use PsExitProcess instead of PsExitThread
		PsExitProcess(ret);
		while (1) ;
	}
	
	UIntPtr idx = 0;																															// Search for this thread in the current process thread list
	Boolean found = False;
	
	ListForeach(PsCurrentProcess->threads, i) {
		if (i->data == PsCurrentThread) {																										// Found?
			found = True;																														// Yes :)
			break;
		} else {
			idx++;																																// Nope
		}
	}
	
	if (found) {																																// Found?
		ListRemove(PsCurrentProcess->threads, idx);																								// Yes!
	}
	
	if (PsWaittList != Null) {																													// Let's wake up any thread that is waiting for us
		for (PListNode i = PsWaittList->tail; i != Null; i = i->prev) {
			PThread th = (PThread)i->data;
			
			if (th->waitt == PsCurrentThread) {
				th->retv = ret;
				PsWakeup(PsWaittList, th);
			}
			
			if (i == PsWaittList->head) {
				break;
			}
		}
	}
	
	PsCurrentThread->prev->next = PsCurrentThread->next;																						// Unqueue
	PsCurrentThread->next->prev = PsCurrentThread->prev;
	
	PsFreeThreadPrivateData(PsCurrentThread->priv);																								// Free the priv data
	MemFree((UIntPtr)PsCurrentThread);																											// Free the thread struct
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(Null);																															// Switch
	
	while (1) ;
}

Void PsExitProcess(UIntPtr ret) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null)) {																				// Sanity checks
		return;
	} else if (PsCurrentProcess->id == 0) {																										// Kernel process?
		PsLockTaskSwitch(old);																													// Yes, so PANIC!
		DbgWriteFormated("PANIC! Tried to close the kernel process\r\n");
		while (1) ;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	ListForeach(PsCurrentProcess->files, i) {
		PProcessFile pf = (PProcessFile)i->data;
		FsCloseFile(pf->file);
		MemFree((UIntPtr)pf);
	}
	
	ListForeach(PsCurrentProcess->threads, i) {																									// Alright, now wakeup (if we need) and free all the threads of this process
		PThread th = (PThread)i->data;
		
		if (PsWaittList != Null) {																												// From the wait thread list
			for (PListNode j = PsWaittList->tail; j != Null; j = j->prev) {
				PThread th2 = (PThread)j->data;
				
				if (th2->waitt == th) {
					th2->retv = ret;
					PsWakeup(PsWaittList, th2);
				} else if (th2 == th) {
					PsWakeup(PsWaittList, th2);
				}
				
				if (j == PsWaittList->head) {
					break;
				}
			}
		}
		
		if (PsWaitpList != Null) {																												// From the wait process list
			ListForeach(PsWaitpList, j) {
				if (j->data == th) {
					PsWakeup(PsWaitpList, (PThread)j->data);
				}
			}
		}
		
		th->prev->next = th->next;																												// Unqueue
		th->next->prev = th->prev;
		
		PsFreeThreadPrivateData(th->priv);																										// Free the priv data
		MemFree((UIntPtr)th);																													// And the thread struct
	}
	
	if (PsWaitpList != Null) {																													// Let's wakeup any thread that is waiting for us
		for (PListNode i = PsWaitpList->tail; i != Null; i = i->prev) {
			PThread th = (PThread)i->data;
			
			if (th->waitp == PsCurrentProcess) {
				th->retv = ret;
				PsWakeup(PsWaitpList, th);
			}
			
			if (i == PsWaitpList->head) {
				break;
			}
		}
	}
	
	if (PsProcessList != Null) {																												// Let's (try) to remove ourself from the process list
		UIntPtr idx = 0;
		Boolean found = False;																													// Let's try to find in the list
		
		ListForeach(PsProcessList, i) {
			if (i->data == PsCurrentProcess) {																									// Found?
				found = True;																													// YES!
				break;
			} else {
				idx++;																															// nope
			}
		}
		
		if (found) {																															// Found?
			ListRemove(PsProcessList, idx);																										// Yes, remove it!
		}
	}
	
	MmFreeDirectory(PsCurrentProcess->dir);																										// Free our page directory
	ListFree(PsCurrentProcess->files);
	ListFree(PsCurrentProcess->threads);																										// Free our thread list
	MemFree((UIntPtr)PsCurrentProcess->name);																									// Free the name
	MemFree((UIntPtr)PsCurrentProcess);																											// And the current process itself
	
	PsTaskSwitchEnabled = olde;																													// DON'T SWITCH BACK TO THE OLD PAGE DIRECTORY!
	PsSwitchTask(Null);																															// Switch to the next process
	
	while (1) ;
}

Void PsInit(Void) {
	PsProcessList = ListNew(False, False);																										// Try to init the process list
	
	if (PsProcessList == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PsWaittList = ListNew(False, False);																										// Try to init the waitt list
	
	if (PsWaittList == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PsWaitpList = ListNew(False, False);																										// Try to init the waitp list
	
	if (PsWaitpList == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PProcess proc = PsCreateProcessInt("System", (UIntPtr)KernelMainLate, MmKernelDirectory);													// Try to create the system process
	
	if (proc == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PsCurrentThread = ListGet(proc->threads, 0);																								// And, get the first thread!	
	
	if (PsCurrentThread == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PsInitInt();																																// Switch to the first thread!
}
