// File author is Ítalo Lima Marconato Matias
//
// Created on August 11 of 2018, at 14:33 BRT
// Last edited on August 15 of 2018, at 20:16 BRT

#include <chicago/arch/uart.h>

Void ArchPreInitDisplay(Void) { }
Void ArchInitPMM(Void) { }
Void ArchInitDisplay(Void) { }
Void MmBootAlloc(Void) { }
Void MmMap(Void) { }
Void MmGetPhys(Void) { }
Void MmUnmap(Void) { }
Void PsCreateThreadPrivateData(Void) { }
Void MmCloneDirectory(Void) { }
Void MmGetCurrentDirectory(Void) { }
UIntPtr MmKernelDirectoryInt = 0;
Void MmSwitchDirectory(Void) { }
Void PsFreeThreadPrivateData(Void) { }
Void PsSwitchTask(Void) { }
Void TimerSleep(Void) { }
Void PsInitInt(Void) { }
Void MmFreeDirectory(Void) { }

Void ArchInitFPU(Void) {
	Asm Volatile("ldr r0, =(0xF << 20)");				// Enable FPU in coprocessor enable register
	Asm Volatile("mcr p15, 0, r0, c1, c0, 2");
	Asm Volatile("mov r3, #0x40000000");				// Enable FPU in FP exception register
	Asm Volatile("vmsr fpexc, r3");
}

Void ArchInitDebug(Void) {
	if (UARTInit != Null) {								// UARTInit exists?
		UARTInit();										// Yes, so call it
	}
}

Void ArchInit(Void) {
	while (1) ;
}
