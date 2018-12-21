// File author is Ítalo Lima Marconato Matias
//
// Created on October 22 of 2018, at 15:31 BRT
// Last edited on December 19 of 2018, at 23:25 BRT

.code16

.extern LoaderMain
.global LoaderEntry
LoaderEntry:
	cli
	
	xor %ax, %ax						// Setup segments
	mov %ax, %ds
	mov %ax, %es
	mov $0x9000, %ax
	mov %ax, %ss
	mov $0xFFFF, %sp
	
	movb %dl, (SubarchBootDrive)		// Save the boot drive
	
	call LoaderEnableA20				// Enable the A20 gate
	
	call LoaderInstallGDT				// Install a basic GDT
	
	mov %cr0, %eax						// Set the pmode bit in CR0
	or $1, %eax
	mov %eax, %cr0
	
	ljmp $0x08, $1f						// Far jump to enter pmode
.code32
1:
	mov $0x10, %ax						// Setup segments
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov $0x90000, %esp
	
	call LoaderMain						// Call the loader main function
2:
	jmp 2b								// Infinite loop

.code16

LoaderEnableA20:
	pusha								// Save all the registers
	
	call 1f								// Disable keyboard
	mov $0xAD, %al
	out %al, $0x64
	
	call 1f								// Tell the controller to read the output port
	mov $0xD0, %al
	out %al, $0x64
	
	call 2f								// Get the output port data and store it
	in $0x60, %al
	push %eax
	
	call 1f								// Tell controller to write to the output port
	mov $0xD1, %al
	out %al, $0x64
	
	call 1f								// Enable A20
	pop %eax
	or $2, %al
	out %al, $0x60
	
	call 1f								// Enable the keyboard
	mov $0xAE, %al
	out %al, $0x64
	
	call 1f
	
	popa								// Restore all the registers
	ret
1:
	in $0x64, %al
	test $2, %al
	jnz 1b
	ret
2:
	call 1b
	mov $0xD0, %al
	out %al, $0x64
3:
	in $0x64, %al
	test $1, %al
	jz 3b
	in $0x60, %al
	ret

LoaderInstallGDT:
	pusha
	lgdt (LoaderGDTPointer)
	popa
	ret

LoaderGDTStart:
	.int 0x00000000, 0x00000000
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0x0F, 0x00
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0x0F, 0x00
LoaderGDTEnd:
LoaderGDTPointer:
	.word LoaderGDTEnd - LoaderGDTStart - 1
	.int LoaderGDTStart

.global SubarchBootDrive
SubarchBootDrive: .byte 0
