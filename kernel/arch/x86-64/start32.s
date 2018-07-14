// File author is Ítalo Lima Marconato Matias
//
// Created on July 11 of 2018, at 01:40 BRT
// Last edited on July 12 of 2018, at 23:49 BRT

.section .multiboot

.align 4
KernelMultibootHeader:													// Multiboot header
.long 0x1BADB002
.long (1 << 0) | (1 << 1) | (1 << 16)
.long -(0x1BADB002 + ((1 << 0) | (1 << 1) | (1 << 16)))
.long (KernelMultibootHeader - 0xFFFFFFFF80000000)
.long (KernelStart - 0xFFFFFFFF80000000)
.long (KernelBssStart - 0xFFFFFFFF80000000)
.long (KernelEnd - 0xFFFFFFFF80000000)
.long (KernelEntry32 - 0xFFFFFFFF80000000)

.section .text

.code32

.extern KernelEntry
.global KernelEntry32
KernelEntry32:
	cli																	// Clear interrupts
	
	movl %eax, (MultibootHeaderMagic - 0xFFFFFFFF80000000)				// Save the multiboot magic and pointer
	movl %ebx, (MultibootHeaderPointer32 - 0xFFFFFFFF80000000)
	
	pushfl																// Check for CPUID support
	popl %eax
	movl %eax, %ecx
	xorl $1 << 21, %eax
	pushl %eax
	popfl
	pushfl
	popl %eax
	pushl %eax
	popfl
	cmpl %ecx, %eax
	je 2f
	
	movl $0x80000000, %eax												// Check for x86-64 long mode support
	cpuid
	cmpl $0x80000001, %eax
	jb 3f
	
	movl $0x80000001, %eax
	cpuid
	testl $1 << 29, %edx
	jz 3f
	
	movl %cr4, %eax
	orl $1 << 5, %eax
	movl %eax, %cr4
	
	movl $(KernelBootP4Table - 0xFFFFFFFF80000000), %eax				// Setup (REQUIRED) initial x86-64 paging and enable IA-32e mode
	movl %eax, %cr3
	
	movl $0xC0000080, %ecx
	rdmsr
	orl $1 << 8, %eax
	wrmsr
	
	movl %cr0, %eax
	orl $1 << 31, %eax
	movl %eax, %cr0
	
	lgdt (KernelBootGDTPointer - 0xFFFFFFFF80000000)					// Load x86-64 gdt
	
	ljmp $0x08, $(4f - 0xFFFFFFFF80000000)								// Long jump to get out of the compatibility mode
0:
	pause
	jmp 0b
1:
	lodsb
	testb %al, %al
	jz 0b
	stosw
	jmp 1b
2:
	movb $0x07, %ah
	movl $0xB8000, %edi
	movl $(KernelBootNoCPUID - 0xFFFFFFFF80000000), %esi
	jmp 1b
3:
	movb $0x07, %ah
	movl $0xB8000, %edi
	movl $(KernelBootNo64 - 0xFFFFFFFF80000000), %esi
	jmp 1b
.code64
4:
	movw $0, %ax														// Setup segments
	movw %ax, %ss
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	
	movabsq $5f, %rax													// Force non-relative jump to enter in the higher half kernel!
	jmp *%rax
5:
	movq $0, KernelBootP4Table + 0										// Unmap the lower half kernel
	invlpg 0
	
	movq $KernelStack, %rsp												// Setup kernel stack
	
	addq $0xFFFFFFFF80000000, (MultibootHeaderPointer32)				// Fix the multiboot header pointer (only the main header address, we're going to fix the entries in the C code)
	
	jmp KernelEntry														// Now the KernelEntry (in start64.s) should handle everything!
.code32

.section .data

.global MultibootHeaderMagic
.global MultibootHeaderPointer
.global MultibootHeaderPointer32
MultibootHeaderMagic:
	.quad 0x0000000000000000
MultibootHeaderPointer:
	.quad 0x0000000000000000
MultibootHeaderPointer32:
	.quad 0x0000000000000000

.section .rodata

KernelBootNoCPUID: .string "PANIC! This CPU doesn't support CPUID instruction"
KernelBootNo64: .string "PANIC! This CPU doesn't support the 64-bit mode"

KernelBootGDT:
	.quad 0x0000000000000000
	.quad 0x0020980000000000
	.quad 0x0000900000000000
KernelBootGDTPointer:
	.word KernelBootGDTPointer - KernelBootGDT - 1
	.quad KernelBootGDT - 0xFFFFFFFF80000000

.align 4096

KernelBootP4Table:
	.quad KernelBootP3Table - 0xFFFFFFFF80000000 + 3
	.fill 507, 8, 0
	.quad KernelBootP4Table - 0xFFFFFFFF80000000 + 3
	.quad 0
	.quad 0
	.quad KernelBootP3Table - 0xFFFFFFFF80000000 + 3
KernelBootP3Table:
	.quad KernelBootP2Table - 0xFFFFFFFF80000000 + 3
	.fill 509, 8, 0
	.quad KernelBootP2Table - 0xFFFFFFFF80000000 + 3
	.fill 1, 8, 0
KernelBootP2Table:
	.quad 0x0000000000000083
	.quad 0x0000000000200083
	.fill 510, 8, 0

.section .bss

.align 16

.skip 8192										// 8 KiB for kernel stack
.global KernelStack
KernelStack:
