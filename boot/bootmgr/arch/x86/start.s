// File author is Ítalo Lima Marconato Matias
//
// Created on October 22 of 2018, at 15:31 BRT
// Last edited on October 23 of 2018, at 16:01 BRT

.code16

.extern LoaderMain
.global LoaderEntry
LoaderEntry:
	cli
	
	mov $0, %ax							// Setup segments
	mov %ax, %ds
	mov %ax, %es
	mov $0x9000, %ax
	mov %ax, %ss
	mov $0xFFFF, %sp
	
	movb %dl, (LoaderBootDrive)
	
	mov $0x1000, %di					// Save the memory map
	call LoaderGetMemoryMap
	
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

LoaderGetMemoryMap:
	pusha								// Save all registers
	xor %ebx, %ebx						// EBX must be 0 to start
	xor %bp, %bp						// We're going to keep the entry count in BP
	mov $0x534D4150, %edx				// Place "SMAP" into EDX
	mov $0xE820, %eax
	movl $1, %es:20(%di)				// Force a valid ACPI 3.x entry
	mov $24, %ecx						// Ask for 24 bytes
	int $0x15
	jc 5f								// Unsupported function?
	mov $0x534D4150, %edx				// Place "SMAP" into EDX
	cmp %edx, %eax						// On success EAX will be "SMAP"
	jne 5f
	test %ebx, %ebx						// 1 entry long list?
	je 5f
	jmp 2f
1:
	mov $0xE820, %eax
	movl $1, %es:20(%di)				// Force a valid ACPI 3.x entry
	mov $24, %ecx
	int $0x15
	jc 1b
	mov $0x534D4150, %edx				// Place "SMAP" into EDX
2:
	jcxz 4f								// Skip 0-length entries
	cmp $20, %cl						// Got a 24 byte ACPI 3.x response?
	jbe 3f
	testb $1, %es:20(%di)
	je 4f
3:
	mov %es:8(%di), %ecx
	or %es:12(%di), %ecx
	jz 4f
	inc %bp
	add $24, %bp
4:
	test %ebx, %ebx						// If EBX is 0, the list is complete!
	jne 1b
	popa
	clc
	ret
5:
	popa
	stc
	ret

LoaderEnableA20:
	pusha								// Save all registers
	
	call 1f								// Disable keyboard
	mov $0xAD, %al
	out %al, $0x64
	call 1f
	
	mov $0xD0, %al						// Tell controller to read output port
	out %al, $0x64
	call 2f
	
	in $0x64, %al						// Get output port data and store it
	push %eax
	call 1f
	
	mov $0xD1, %al						// Tell controller to write output port
	out %al, $0x64
	call 1f
	
	pop %eax							// Enable the A20 gate
	or $2, %al
	out %al, $0x60
	
	call 1f								// Re-enable keyboard
	mov $0xAE, %al
	out %al, $0x64
	call 1f
	
	popa
	ret
1:
	in $0x64, %al
	test $2, %al
	jnz 1b
	ret
2:
	in $0x64, %al
	test $1, %al
	jz 2b
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
LoaderGDTEnd:
LoaderGDTPointer:
	.word LoaderGDTEnd - LoaderGDTStart - 1
	.int LoaderGDTStart

.global LoaderBootDrive
LoaderBootDrive: .byte 0
