// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:18 BRT
// Last edited on May 12 of 2018, at 09:01 BRT

.section .text

.extern __bss_start
.extern __bss_end
.extern KernelMain
.global KernelEntry
KernelEntry:
	mov sp, #0x8000							// Setup kernel stack
	
	ldr r4, =__bss_start					// Clear out bss
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b 2f
1:
	stmia r4!, {r5-r8}						// Store multiple at r4
2:
	cmp r4, r9								// If we are still below bss_end, loop
	blo 1b
	
	ldr r3, =KernelMain						// Go to main kernel function
	blx r3
.Loop:
	wfe
	b .Loop
