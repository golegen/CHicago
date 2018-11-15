// File author is Ítalo Lima Marconato Matias
//
// Created on July 27 of 2018, at 14:59 BRT
// Last edited on November 15 of 2018, at 16:02 BRT

#define __CHICAGO_PROCESS__

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/panic.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/timer.h>
#include <chicago/list.h>
#include <chicago/queue.h>
#include <chicago/virt.h>

extern Void KernelMainLate(Void);

Boolean PsTaskSwitchEnabled = False;
PProcess PsCurrentProcess = Null;
PQueue PsProcessQueue = Null;
PList PsProcessList = Null;
UIntPtr PsNextID = 0;

PProcess PsCreateProcessInt(PChar name, UIntPtr entry, UIntPtr dir) {
	PProcess proc = (PProcess)MemAllocate(sizeof(Process));																						// Let's try to allocate the process struct!
	
	if (proc == Null) {
		return Null;																															// Failed
	}
	
	proc->id = PsNextID++;																														// Use the next PID
	proc->name = StrDuplicate(name);																											// Duplicate the name
	
	if ((proc->dir = dir) == 0) {																												// Create an new page dir?
		proc->dir = MmCreateDirectory();																										// Yes
		
		if (proc->dir == 0) {
			MemFree((UIntPtr)proc->name);																										// Failed...
			MemFree((UIntPtr)proc);
			
			return Null;
		}
	}
	
	if ((proc->ctx = PsCreateContext(entry)) == Null) {																							// Create the process context
		MmFreeDirectory(proc->dir);																												// Failed...
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
	}
	
	proc->alloc_base = Null;
	proc->mem_usage = 0;
	proc->handle_list = Null;
	proc->global_handle_list = Null;
	
	return proc;
}

PProcess PsCreateProcess(PChar name, UIntPtr entry) {
	return PsCreateProcessInt(name, entry, 0);																									// Use our PsCreateProcessInt function
}

Void PsAddProcess(PProcess proc) {
	if ((proc == Null) || (proc->dir == 0) || (PsProcessQueue == Null) || (PsProcessList == Null)) {											// Sanity checks
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	QueueAdd(PsProcessQueue, proc);																												// Try to add the proc to the queue
	ListAdd(PsProcessList, proc);																												// Try to add the proc to the proc list
	PsUnlockTaskSwitch(old);																													// Unlock
}

PProcess PsGetProcess(UIntPtr id) {
	if (PsProcessList == Null) {																												// Sanity checks
		return Null;
	}
	
	ListForeach(PsProcessList, i) {																												// Let's search!
		PProcess proc = (PProcess)i->data;
		
		if (proc->id == id) {																													// Match?
			return proc;																														// Yes :)
		}
	}
	
	return Null;
}

Void PsLock(PLock lock) {
	if ((lock == Null) || (PsCurrentProcess == Null)) {																							// Sanity checks
		return;
	}
	
	while (*lock) {																																// Just keep on checking!
		PsSwitchTask(Null);
	}
	
	*lock = True;
}

Void PsUnlock(PLock lock) {
	if ((lock == Null) || (PsCurrentProcess == Null)) {																							// Sanity checks
		return;
	}
	
	*lock = False;																																// Unlock...
}

Void PsExitProcess(Void) {
	if (PsCurrentProcess == Null) {																												// Sanity checks
		return;
	} else if (PsCurrentProcess->id == 0) {																										// Kernel process?
		PsLockTaskSwitch(old);																													// Yes, so PANIC!
		DbgWriteFormated("PANIC! Tried to close the kernel process\r\n");
		Panic(PANIC_KERNEL_UNEXPECTED_ERROR);
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
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
	
	PsFreeContext(PsCurrentProcess->ctx);																										// Free the context
	MmFreeDirectory(PsCurrentProcess->dir);																										// Free the directory
	MemFree((UIntPtr)PsCurrentProcess->name);																									// Free the name
	MemFree((UIntPtr)PsCurrentProcess);																											// And the current process itself
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(Null);																															// Switch to the next process
	ArchHalt();																																	// Halt
}

Void PsInit(Void) {
	if ((PsProcessQueue = QueueNew(False)) == Null) {																							// Try to init the process queue
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	if ((PsProcessList = ListNew(False, False)) == Null) {																						// Try to init the process list
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	PProcess proc = PsCreateProcessInt("System", (UIntPtr)KernelMainLate, MmKernelDirectory);													// Try to create the system process
	
	if (proc == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	if (!QueueAdd(PsProcessQueue, proc)) {																										// Try to add it to the process queue
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	if (!ListAdd(PsProcessList, proc)) {																										// And to the process list
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	PsTaskSwitchEnabled = True;																													// Enable task switching
	PsSwitchTask(Null);																															// And switch to the system process!
}
