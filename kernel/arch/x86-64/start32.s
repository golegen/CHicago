// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:20 BRT
// Last edited on May 12 of 2018, at 09:03 BRT

.section .multiboot

.align 4										// Multiboot header
.long 0x1BADB002
.long (1 << 0) | (1 << 1)
.long -(0x1BADB002 + ((1 << 0) | (1 << 1)))

.section .text

.code32

.extern KernelEntry
.global KernelEntry32
KernelEntry32:
	cli											// Clear interrupts
	
	movl $kstack, %esp							// Setup kernel stack
	
	call KernelEntry32.CheckCPUID				// Check for CPUID support
	call KernelEntry32.CheckLongMode			// Check for x86-64 long mode support
	
	call KernelEntry32.SetupPageTables			// Setup (REQUIRED) inital x86-64 paging
	call KernelEntry32.EnablePaging
	
	lgdt (KernelEntry32.GDT.Pointer)			// Load x86-64 gdt
	
	ljmp $0x08, $KernelEntry
KernelEntry32.Loop:
	pause
	jmp KernelEntry32.Loop
KernelEntry32.Error:
	movl $0x4F524F45, (0xB8000)
    movl $0x4F3A4F52, (0xB8004)
    movl $0x4F204F20, (0xB8008)
    movb %al, (0xB800A)
    jmp KernelEntry32.Loop
KernelEntry32.CheckCPUID:
	pushfl
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
	je KernelEntry32.CheckCPUID.NoCPUID
	
	ret
KernelEntry32.CheckCPUID.NoCPUID:
	movb $48, %al
	jmp KernelEntry32.Error
KernelEntry32.CheckLongMode:
	movl $0x80000000, %eax
	cpuid
	cmpl $0x80000001, %eax
	jb KernelEntry32.CheckLongMode.NoLongMode
	
	movl $0x80000001, %eax
	cpuid
	testl $1 << 29, %edx
	jz KernelEntry32.CheckLongMode.NoLongMode
	
	ret
KernelEntry32.CheckLongMode.NoLongMode:
	movb $49, %al
	jmp KernelEntry32.Error
KernelEntry32.SetupPageTables:
	movl $KernelEntry32.P3Table, %eax
	orl $0b11, %eax
	movl %eax, (KernelEntry32.P4Table)
	
	mov $KernelEntry32.P2Table, %eax
	orl $0b11, %eax
	movl %eax, (KernelEntry32.P3Table)
	
	movl $0, %ecx
KernelEntry32.SetupPageTables.MapP2Table:
	movl $0x200000, %eax
	mull %ecx
	orl $0b10000011, %eax
	movl %eax, KernelEntry32.P2Table(,%ecx, 8)
	
	incl %ecx
	cmpl $512, %ecx
	jne KernelEntry32.SetupPageTables.MapP2Table
	
	ret
KernelEntry32.EnablePaging:
	movl $KernelEntry32.P4Table, %eax
	movl %eax, %cr3
	
	movl %cr4, %eax
	orl $1 << 5, %eax
	movl %eax, %cr4
	
	movl $0xC0000080, %ecx
	rdmsr
	orl $1 << 8, %eax
	wrmsr
	
	movl %cr0, %eax
	orl $1 << 31, %eax
	movl %eax, %cr0
	
	ret

.section .rodata

KernelEntry32.GDT:
	.quad 0x0000000000000000
	.quad 0x0020980000000000
	.quad 0x0000900000000000
KernelEntry32.GDT.Pointer:
	.word KernelEntry32.GDT.Pointer - KernelEntry32.GDT - 1
	.quad KernelEntry32.GDT
	
.section .bss

.align 4096

KernelEntry32.P4Table:
.skip 4096
KernelEntry32.P3Table:
.skip 4096
KernelEntry32.P2Table:
.skip 4096

.align 16

.skip 8192										// 8 KiB for kernel stack
.global kstack
kstack:
