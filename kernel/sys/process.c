// File author is Ítalo Lima Marconato Matias
//
// Created on July 27 of 2018, at 14:59 BRT
// Last edited on August 05 of 2018, at 13:31 BRT

#define __CHICAGO_PROCESS__

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/timer.h>

extern Void KernelMainLate(Void);

Boolean PsTaskSwitchEnabled = False;
PThread PsCurrentThread = Null;
UIntPtr PsPrivateDataSize = 0;
PList PsSleepList = Null;
PList PsWaittList = Null;
PList PsWaitpList = Null;
PList PsWaitlList = Null;
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
	th->waitl = Null;																															// This will be set in the PsLock function
	th->time = PS_TIMESLICE - 1;																												// Set the default timeslice
	th->retv = 0;																																// This maybe set in the PsExitThread/Process function
	th->wait_time = 0;																															// This will be set in the PsSleep function
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
	proc->threads = ListNew(True);																												// Create the thread list
	
	if (proc->threads == Null) {
		MemFree((UIntPtr)proc->name);																											// Failed...
		MemFree((UIntPtr)proc);
		
		return Null;
	}
	
	PThread th = PsCreateThreadInt(proc, entry);																								// Let's create the first thread!
	
	if (th == Null) {
		ListFree(proc->threads);																												// Failed...
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		
		return Null;
	}
	
	if (!ListAdd(proc->threads, th)) {																											// Let's add it to this proc thread list!
		PsFreeThreadPrivateData(th->priv);
		MemFree((UIntPtr)th);
		ListFree(proc->threads);																												// Failed...
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		
		return Null;
	}
	
	if (proc->dir == 0) {																														// Create an new page dir?
		proc->dir = MmCloneDirectory();																											// Yes
		
		if (proc->dir == 0) {
			PsFreeThreadPrivateData(th->priv);
			ListFree(proc->threads);																											// Failed...
			MemFree((UIntPtr)proc->name);
			MemFree((UIntPtr)proc);
			
			return Null;
		}
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
	
	PsUnlockTaskSwitch(old);																													// Unlock
}

Void PsSleep(UIntPtr ms) {
	if ((PsSleepList == Null) || (PsCurrentThread == Null)) {																					// Thread list (and sleep list) is initialized?
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
	
	if (!ListAdd(PsSleepList, PsCurrentThread)) {																								// Try to add this thread to the sleep list
		PsUnlockTaskSwitch(old);																												// Failed... TimerSleep
		TimerSleep(ms);
		return;
	}
	
	PsCurrentThread->prev->next = PsCurrentThread->next;																						// Dequeue this thread
	PsCurrentThread->next->prev = PsCurrentThread->prev;
	PsCurrentThread->wait_time = ms - 1;																										// Set the wait time
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(Null);																															// Switch!
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
	
	PsCurrentThread->prev->next = PsCurrentThread->next;																						// Unqueue
	PsCurrentThread->next->prev = PsCurrentThread->prev;
	PsCurrentThread->waitt = th;
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(Null);																															// Switch!
	
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
	
	PsCurrentThread->prev->next = PsCurrentThread->next;																						// Unqueue
	PsCurrentThread->next->prev = PsCurrentThread->prev;
	PsCurrentThread->waitp = proc;
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(Null);																															// Switch!
	
	return PsCurrentThread->retv;
}

Void PsLock(PLock lock) {
	if ((lock == Null) || (PsWaitlList == Null) || (PsCurrentThread == Null)) {																	// Sanity checks
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	if ((PsCurrentThread == PsCurrentThread->next) || (!lock->locked)) {																		// We can add it to the wait list? Even better, we need to add it?
		lock->locked = True;																													// Nope, we don't need (or we can't)
		lock->owner = PsCurrentThread;
		PsUnlockTaskSwitch(old);
		return;
	}
	
	if (!ListAdd(PsWaitlList, PsCurrentThread)) {																								// Try to add this thread to the wait list
		PsUnlockTaskSwitch(old);																												// Failed, but we're not going to give up
		PsLock(lock);
	}
	
	PsCurrentThread->prev->next = PsCurrentThread->next;																						// Unqueue
	PsCurrentThread->next->prev = PsCurrentThread->prev;
	PsCurrentThread->waitl = lock;
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(Null);																															// Switch!
}

Void PsUnlock(PLock lock) {
	if ((lock == Null) || (PsWaitlList == Null) || (PsCurrentThread == Null)) {																	// Sanity checks
		return;																																	// ...
	} else if (lock->owner != PsCurrentThread) {																								// We're the owner of this lock?
		return;																																	// Nope
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	lock->locked = False;																														// Unlock the lock
	lock->owner = Null;
	
	ListForeach(PsWaitlList, i) {																												// Let's try to wakeup any thread that is waiting for this lock
		PThread th = (PThread)i->data;
		
		if (th->waitl == lock) {
			PsWakeup(PsWaitlList, th);																											// Found one!
			
			lock->locked = True;																												// Let's lock, set the lock owner, and force switch to it!
			lock->owner = th;
			
			PsUnlockTaskSwitch(old);
			PsSwitchTask(Null);
			
			break;
		}
	}
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(Null);	
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
	
	th->time = PS_TIMESLICE - 1;																												// Queue it back to the thread list
	th->waitl = Null;
	th->waitt = Null;
	th->waitp = Null;
	th->wait_time = 0;
	th->next->prev = th;
	th->prev = PsCurrentThread;
	PsCurrentThread->next = th;
	th->next = PsCurrentThread->next;
	
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
	
	ListForeach(PsCurrentProcess->threads, i) {																									// Alright, now wakeup (if we need) and free all the threads of this process
		PThread th = (PThread)i->data;
		
		if (PsSleepList != Null) {																												// Wakeup from the sleep list
			ListForeach(PsSleepList, j) {
				if (j->data == th) {
					PsWakeup(PsSleepList, (PThread)j->data);
				}
			}
		}
		
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
		
		if (PsWaitlList != Null) {																												// And for the wait lock list
			ListForeach(PsWaitlList, j) {
				if (j->data == th) {
					PsWakeup(PsWaitlList, (PThread)j->data);
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
	
	MmFreeDirectory(PsCurrentProcess->dir);																										// Free our page directory
	ListFree(PsCurrentProcess->threads);																										// Free our thread list
	MemFree((UIntPtr)PsCurrentProcess->name);																									// Free the name
	MemFree((UIntPtr)PsCurrentProcess);																											// And the current process itself
	
	PsTaskSwitchEnabled = olde;																													// DON'T SWITCH BACK TO THE OLD PAGE DIRECTORY!
	PsSwitchTask(Null);																															// Switch to the next process
	
	while (1) ;
}

Void PsInit(Void) {
	PsSleepList = ListNew(False);																												// Try to init the sleep list
	
	if (PsSleepList == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PsWaittList = ListNew(False);																												// Try to init the waitt list
	
	if (PsWaittList == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PsWaitpList = ListNew(False);																												// Try to init the waitp list
	
	if (PsWaitpList == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PsWaitlList = ListNew(False);																												// Try to init the waitl list
	
	if (PsWaitlList == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		while (1) ;
	}
	
	PProcess proc = PsCreateProcessInt("chkrnl.che", (UIntPtr)KernelMainLate, MmKernelDirectory);												// Try to create the initial process
	
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
