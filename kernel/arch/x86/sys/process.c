// File author is Ítalo Lima Marconato Matias
//
// Created on July 28 of 2018, at 01:09 BRT
// Last edited on October 11 of 2018, at 22:23 BRT

#define __CHICAGO_ARCH_PROCESS__

#include <chicago/arch/gdt.h>
#include <chicago/arch/port.h>
#include <chicago/arch/process.h>
#include <chicago/arch/idt.h>

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/timer.h>

Aligned(16) UInt8 PsFPUStateSave[512];
Aligned(16) UInt8 PsFPUDefaultState[512];

PVoid PsCreateThreadPrivateData(UIntPtr entry) {
	PThreadPrivateData priv = (PThreadPrivateData)MemAllocate(sizeof(ThreadPrivateData));							// Alloc some space for the struct
	PUIntPtr kstack = 0;
	
	if (priv == Null) {
		return Null;																								// Failed :(
	}
	
	kstack = (PUIntPtr)(((UIntPtr)priv->kstack) + PS_STACK_SIZE);													// Let's push the stack default state (setup the registers)
	
	*kstack-- = 0x10;																								// Push what we need for using the IRET in the first schedule
	*kstack-- = ((UIntPtr)priv->kstack) + PS_STACK_SIZE;
	*kstack-- = 0x202;
	*kstack-- = 0x08;
	*kstack-- = entry;
	*kstack-- = 0;																									// And all the other registers that we need
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = ((UIntPtr)priv->kstack) + PS_STACK_SIZE;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0x10;
	*kstack-- = 0x10;
	*kstack-- = 0x10;
	*kstack = 0x10;
	
	priv->esp = (UIntPtr)kstack;
	
	StrCopyMemory(priv->fpu_state, PsFPUDefaultState, 512);															// Setup the default fpu state
	
	return priv;
}

Void PsFreeThreadPrivateData(PVoid priv) {
	MemFree((UIntPtr)priv);																							// Just MemFree the priv pointer
}

Void PsSwitchTaskTimer(PRegisters regs) {
	if (PsCurrentThread == Null) {																					// We can switch?
		return;																										// Nope
	}
	
	if (!PsTaskSwitchEnabled) {																						// Task switch enabled?
		return;																										// Nope
	}
	
	if (PsCurrentThread->time != 0) {																				// Switch to the next thread?
		PsCurrentThread->time--;																					// No, just decrease the time slice/quantum
	} else if (PsCurrentThread == PsCurrentThread->next) {
		PsCurrentThread->time = PS_TIMESLICE - 1;																	// Yes, but we only have one thread in the queue list...
	} else {
		PThread old = PsCurrentThread;																				// Yes!
		PThread new = PsCurrentThread->next;
		
		((PThreadPrivateData)(old->priv))->esp = (UIntPtr)regs;														// Save the old regs
		old->time = PS_TIMESLICE - 1;																				// Reset the time slice
		PsCurrentThread = new;
		
		GDTSetKernelStack((UInt32)(((PThreadPrivateData)(new->priv))->kstack));										// Load the kernel stack
		Asm Volatile("fxsave (%0)" :: "r"(PsFPUStateSave));															// Save the old fp state
		StrCopyMemory(((PThreadPrivateData)(old->priv))->fpu_state, PsFPUStateSave, 512);
		StrCopyMemory(PsFPUStateSave, ((PThreadPrivateData)(new->priv))->fpu_state, 512);							// And load the new one!
		Asm Volatile("fxrstor (%0)" :: "r"(PsFPUStateSave));
		
		if (new->parent != old->parent) {																			// Same process?
			MmSwitchDirectory(new->parent->dir);																	// Nope, load the new other page directory
		}
		
		PortOutByte(0x20, 0x20);																					// Send EOI
		Asm Volatile("mov %%eax, %%esp" :: "a"(((PThreadPrivateData)(new->priv))->esp));							// LET'S SWITCH!
		Asm Volatile("pop %gs");
		Asm Volatile("pop %fs");
		Asm Volatile("pop %es");
		Asm Volatile("pop %ds");
		Asm Volatile("pop %edi");
		Asm Volatile("pop %esi");
		Asm Volatile("pop %ebp");
		Asm Volatile("pop %ebx");
		Asm Volatile("pop %edx");
		Asm Volatile("pop %ecx");
		Asm Volatile("pop %eax");
		Asm Volatile("add $8, %esp");
		Asm Volatile("iret");
	}
}

Void PsSwitchTaskForce(PRegisters regs) {
	if ((!PsTaskSwitchEnabled) || (PsCurrentThread == Null)) {														// We can switch?
		return;																										// Nope
	} else if (PsCurrentThread == PsCurrentThread->next) {															// We have anything to switch into?
		return;																										// Nope
	}
	
	PThread old = PsCurrentThread;
	PThread new = PsCurrentThread->next;
	
	((PThreadPrivateData)(old->priv))->esp = (UIntPtr)regs;															// Save the old regs
	new->time += old->time;																							// Donate the time slice to the new thread
	old->time = PS_TIMESLICE - 1;																					// Reset the old time slice
	PsCurrentThread = new;
	
	GDTSetKernelStack((UInt32)(((PThreadPrivateData)(new->priv))->kstack));											// Load the kernel stack
	Asm Volatile("fxsave (%0)" :: "r"(PsFPUStateSave));																// Save the old fp state
	StrCopyMemory(((PThreadPrivateData)(old->priv))->fpu_state, PsFPUStateSave, 512);
	StrCopyMemory(PsFPUStateSave, ((PThreadPrivateData)(new->priv))->fpu_state, 512);								// And load the new one!
	Asm Volatile("fxrstor (%0)" :: "r"(PsFPUStateSave));
	
	if (new->parent != old->parent) {																				// Same process?
		MmSwitchDirectory(new->parent->dir);																		// Nope, load the new other page directory
	}
	
	Asm Volatile("mov %%eax, %%esp" :: "a"(((PThreadPrivateData)(new->priv))->esp));								// LET'S SWITCH!
	Asm Volatile("pop %gs");
	Asm Volatile("pop %fs");
	Asm Volatile("pop %es");
	Asm Volatile("pop %ds");
	Asm Volatile("pop %edi");
	Asm Volatile("pop %esi");
	Asm Volatile("pop %ebp");
	Asm Volatile("pop %ebx");
	Asm Volatile("pop %edx");
	Asm Volatile("pop %ecx");
	Asm Volatile("pop %eax");
	Asm Volatile("add $8, %esp");
	Asm Volatile("iret");
}

Void PsSwitchTask(PVoid priv) {
	if (PsCurrentThread == Null) {																					// We can switch?
		return;																										// Nope
	}
	
	Boolean olde = PsTaskSwitchEnabled;
	
	PsTaskSwitchEnabled = False;																					// Disable task switch
	
	if (MmGetCurrentDirectory() != MmKernelDirectory) {																// Need to switch to the kernel directory?
		MmSwitchDirectory(MmKernelDirectory);																		// Yes
	}
	
	if (priv != Null) {																								// Timer?
		PsTaskSwitchEnabled = olde;																					// Yes
		PsSwitchTaskTimer((PRegisters)priv);
	} else {
		PsTaskSwitchEnabled = olde;																					// Nope, so let's use int num 0x3E
		Asm Volatile("int $0x3E");
	}
}

Void PsInitInt(Void) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null)) {													// Failed to create the initial thread and process?
		DbgWriteFormated("PANIC! Couldn't init tasking\r\n");														// Yes :(
		while (1) ;
	}
	
	IDTRegisterInterruptHandler(0x3E, PsSwitchTaskForce);															// Register the force task switch handler
	GDTSetKernelStack((UInt32)(((PThreadPrivateData)(PsCurrentThread->priv))->kstack));								// Load the kernel stack
	StrCopyMemory(PsFPUStateSave, ((PThreadPrivateData)(PsCurrentThread->priv))->fpu_state, 512);					// Load the intial fpu state
	Asm Volatile("fxrstor (%0)" :: "r"(PsFPUStateSave));
	
	if (MmGetCurrentDirectory() != PsCurrentProcess->dir) {															// Switch the page directory if required
		MmSwitchDirectory(PsCurrentProcess->dir);
	}
	
	Asm Volatile("mov %%eax, %%esp" :: "a"(((PThreadPrivateData)(PsCurrentThread->priv))->esp));					// Let's switch to our first thread!
	Asm Volatile("pop %gs");
	Asm Volatile("pop %fs");
	Asm Volatile("pop %es");
	Asm Volatile("pop %ds");
	Asm Volatile("pop %edi");
	Asm Volatile("pop %esi");
	Asm Volatile("pop %ebp");
	Asm Volatile("pop %ebx");
	Asm Volatile("pop %edx");
	Asm Volatile("pop %ecx");
	Asm Volatile("pop %eax");
	Asm Volatile("add $8, %esp");
	Asm Volatile("iret");
}
