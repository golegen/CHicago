// File author is Ítalo Lima Marconato Matias
//
// Created on July 28 of 2018, at 01:09 BRT
// Last edited on November 10 of 2018, at 12:39 BRT

#define __CHICAGO_ARCH_PROCESS__

#include <chicago/arch/gdt.h>
#include <chicago/arch/port.h>
#include <chicago/arch/process.h>
#include <chicago/arch/idt.h>

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/panic.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/timer.h>

Aligned(16) UInt8 PsFPUStateSave[512];
Aligned(16) UInt8 PsFPUDefaultState[512];

PContext PsCreateContext(UIntPtr entry) {
	PContext ctx = (PContext)MemAllocate(sizeof(Context));															// Alloc some space for the context struct
	
	if (ctx == Null) {
		return Null;																								// Failed :(
	}
	
	PUIntPtr kstack = (PUIntPtr)(ctx->kstack + PS_STACK_SIZE - 1);													// Let's setup the context registers!
	
	*kstack-- = 0x10;																								// Push what we need for using the IRET in the first schedule
	*kstack-- = (UIntPtr)(ctx->kstack + PS_STACK_SIZE - 1);
	*kstack-- = 0x202;
	*kstack-- = 0x08;
	*kstack-- = entry;
	*kstack-- = 0;																									// And all the other registers that we need
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = (UIntPtr)(ctx->kstack + PS_STACK_SIZE - 1);
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0;
	*kstack-- = 0x10;
	*kstack-- = 0x10;
	*kstack-- = 0x10;
	*kstack = 0x10;
	
	ctx->esp = (UIntPtr)kstack;
	
	StrCopyMemory(ctx->fpu_state, PsFPUDefaultState, 512);															// Setup the default fpu state
	
	return ctx;
}

Void PsFreeContext(PContext ctx) {
	MemFree((UIntPtr)ctx);																							// Just MemFree the ctx!
}

Void PsSwitchTaskTimer(PRegisters regs) {
	if ((PsProcessQueue == Null) || (PsProcessQueue->length == 0) || (!PsTaskSwitchEnabled)) {						// We can switch?
		return;																										// Nope
	}
	
	PProcess old = PsCurrentProcess;																				// Save the old process
	
	PsCurrentProcess = QueueRemove(PsProcessQueue);																	// Get the next process
	old->ctx->esp = (UIntPtr)regs;																					// Save the old context
	
	QueueAdd(PsProcessQueue, old);																					// Add the old process to the queue again
	GDTSetKernelStack((UInt32)(PsCurrentProcess->ctx->kstack));														// Switch the kernel stack in the tss
	Asm Volatile("fxsave (%0)" :: "r"(PsFPUStateSave));																// Save the old fpu state
	StrCopyMemory(old->ctx->fpu_state, PsFPUStateSave, 512);
	StrCopyMemory(PsFPUStateSave, PsCurrentProcess->ctx->fpu_state, 512);											// And load the new one
	Asm Volatile("fxrstor (%0)" :: "r"(PsFPUStateSave));
	
	if (PsCurrentProcess->dir != old->dir) {																		// Switch the page dir
		MmSwitchDirectory(PsCurrentProcess->dir);
	}
	
	PortOutByte(0x20, 0x20);																						// Send EOI
	Asm Volatile("mov %%eax, %%esp" :: "a"(PsCurrentProcess->ctx->esp));											// And let's switch!
	Asm Volatile("pop %gs");
	Asm Volatile("pop %fs");
	Asm Volatile("pop %es");
	Asm Volatile("pop %ds");
	Asm Volatile("popa");
	Asm Volatile("add $8, %esp");
	Asm Volatile("iret");
}

Void PsSwitchTaskForce(PRegisters regs) {
	if ((PsProcessQueue == Null) || (PsProcessQueue->length == 0) || (!PsTaskSwitchEnabled)) {						// We can switch?
		return;																										// Nope
	}
	
	PProcess old = PsCurrentProcess;																				// Save the old process
	
	PsCurrentProcess = QueueRemove(PsProcessQueue);																	// Get the next process
	
	if (old != Null) {																								// Save the old proc info?
		old->ctx->esp = (UIntPtr)regs;																				// Yes, save the old context
		Asm Volatile("fxsave (%0)" :: "r"(PsFPUStateSave));															// And the old fpu state
		StrCopyMemory(old->ctx->fpu_state, PsFPUStateSave, 512);
		QueueAdd(PsProcessQueue, old);																				// And add the old process to the queue again
	}
	
	GDTSetKernelStack((UInt32)(PsCurrentProcess->ctx->kstack));														// Switch the kernel stack in the tss
	StrCopyMemory(PsFPUStateSave, PsCurrentProcess->ctx->fpu_state, 512);											// And load the new fpu state
	Asm Volatile("fxrstor (%0)" :: "r"(PsFPUStateSave));
	
	if (((old != Null) && (PsCurrentProcess->dir != old->dir)) || (old == Null)) {									// Switch the page dir
		MmSwitchDirectory(PsCurrentProcess->dir);
	}
	
	Asm Volatile("mov %%eax, %%esp" :: "a"(PsCurrentProcess->ctx->esp));											// And let's switch!
	Asm Volatile("pop %gs");
	Asm Volatile("pop %fs");
	Asm Volatile("pop %es");
	Asm Volatile("pop %ds");
	Asm Volatile("popa");
	Asm Volatile("add $8, %esp");
	Asm Volatile("iret");
}

Void PsSwitchTask(PVoid priv) {
	if ((PsProcessQueue == Null) || (PsProcessQueue->length == 0) || (!PsTaskSwitchEnabled)) {						// We can switch?
		return;																										// Nope
	} else if (priv != Null) {																						// Use timer?
		PsSwitchTaskTimer((PRegisters)priv);																		// Yes!
	} else {
		Asm Volatile("int $0x3E");																					// Nope, so let's use int 0x3E
	}
}
