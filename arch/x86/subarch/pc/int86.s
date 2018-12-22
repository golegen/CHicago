// File author is Ítalo Lima Marconato Matias
//
// Created on October 26 of 2018, at 18:36 BRT
// Last edited on October 26 of 2018, at 21:03 BRT

.code32

Int86Num: .byte 0
Int86SaveIDT: .quad 0
Int86SaveStack: .int 0
Int86RealModeIDT: .word 0x3FF, 0, 0

.global Int86
Int86:
	pusha								// Save the registers
	mov %esp, (Int86SaveStack)			// Save the 32-bits stack
	
	push %eax							// We're going to use the EAX
	mov 40(%esp), %eax					// Get the INT num
	movb %al, (Int86Num)				// Write it to the temp var
	pop %eax
	
	mov 40(%esp), %esp					// Load the 16-bits stack
	
	cli									// Disable the interrupts
	
	sidt (Int86SaveIDT)					// Save the 32-bits IDT
	lidt (Int86RealModeIDT)				// Load the 16-bits IDT
	
	ljmp $0x18, $1f						// Long jump to enter 16-bits protected mode
.code16
1:
	mov $0x20, %eax						// Set the 16-bits GDT segments
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %ss
	
	mov %cr0, %eax
	and $0xFE, %eax						// Unset the pmode bit in the CR0
	mov %eax, %cr0
	
	ljmp $0x00, $2f						// Long jump to enter real mode
2:
	mov $0x00, %ax						// Set the 16-bits segments
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	
	popal								// Get the input from the 16-bits stack
	sti									// Restore the interrupts
	
	push %ax							// We're going to use AX
	mov (Int86Num), %al					// Get the INT num
	mov %al, (3f)						// Overwrite the dummy number
	pop %ax
	
	.byte 0xCD							// INT instruction
3:	.byte 0x00
	
	cli									// Disable the interrupts
	pushal								// Write the output to the 16-bits stack
	
	mov %cr0, %eax
	or $1, %eax							// Set the pmode bit in the CR0
	mov %eax, %cr0
	
	ljmp $0x08, $4f						// Long jump to enter pmode
.code32
4:
	mov $0x10, %eax						// Restore the 32-bits segments
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %ss
	mov (Int86SaveStack), %esp			// Load the 32-bits stack
	
	lidt (Int86SaveIDT)					// Load the 32-bits IDT
	
	sti									// Re-enable the interrupts
	popa								// Restore the registers
	ret
