// File author is Ítalo Lima Marconato Matias
//
// Created on October 22 of 2018, at 13:36 BRT
// Last edited on October 24 of 2018, at 18:47 BRT

.code16

.global BootEntry
BootEntry:
	cli
	
	xor %ax, %ax												// Setup segments
	mov %ax, %ds
	mov %ax, %es
	mov $0x9000, %ax
	mov %ax, %ss
	mov $0xFFFF, %sp
	
	sti
	
	movb %dl, (BootDrive)										// Save the boot drive
	
	call LoadRootDirectory										// Load the root directory
	
	mov $BootFolder, %si										// Load the Boot folder
	mov $0x01, %al												// It's a folder
	mov $0x04, %ah
	call LoadFile
	
	mov $BootmgrFile, %si										// Load the bootmgr.bin file
	xor %al, %al												// It's a file
	mov $0x0B, %ah
	call LoadFile
	
	ljmp $0x00, $0x7E00											// Long jump to it!
1:
	hlt
	jmp 1b

LoadRootDirectory:
	mov $0x10, %ebx												// Start at the sector 0x10
	mov $0x01, %ecx												// Just one sector at time
	mov $0x7E00, %di											// Load to 0x7E00
1:
	call ReadSectors											// Read the sector
	
	mov %es:(%di), %al											// Get type
	cmp $0x01, %al												// Found?
	je 2f														// Yes!
	cmp $0xFF, %al												// Terminator?
	je 3f														// Yes...
	
	inc %ebx													// Move to the next sector
	jmp 1b
2:
	movl %es:158(%di), %ebx										// Return the root directory sector in BX
	ret
3:
	mov $RootDirErrMsg, %si
	call WriteString
	call Reboot

FolderFlag: .byte 0
FileNameLength: .byte 0
FileName: .word 0
ReturnValue: .int 0
LoadFile:
	pusha														// Save all the registers
	movb %al, (FolderFlag)										// Save the folder flag
	movb %ah, (FileNameLength)									// Save the file name length
	movw %si, (FileName)										// Save the file name
1:
	mov $0x01, %ecx												// Just one sector at time
	mov $0x7E00, %di											// Load to 0x7E00
	call ReadSectors											// Load the current dir sector into buffer
2:
	xor %ax, %ax												// Terminator?
	mov %es:(%di), %al
	cmp $0, %al
	je 11f														// Yes, error out...
	
	cmpb $0x01, (FolderFlag)									// We're searching for a folder?
	je 3f
	
	testb $0x02, %es:25(%di)									// It's a file?
	jnz 5f														// No...
	
	jmp 4f
3:
	testb $0x02, %es:25(%di)									// It's a folder?
	jz 5f														// No...
4:
	mov (FileNameLength), %cl									// Same filename length?
	cmpb %cl, %es:32(%di)
	jne 5f														// No...
	
	push %di													// Let's compare the names!
	add $33, %di
	cld
	rep cmpsb
	pop %di
	je 7f														// Equals?
5:
	add %ax, %di												// No...
	cmp $0x8600, %di
	jb 2b														// Go to next sector?
6:
	inc %ebx													// Yes
	jmp 1b
7:
	cmpb $0x01, (FolderFlag)									// FOUND! It's a folder?
	jne 8f
	
	movl %es:2(%di), %ax										// Yes, let's return the folder start sector at BX
	movl %eax, (ReturnValue)
	jmp 10f
8:
	movl %es:2(%di), %ebx										// No, so let's load the file
	movl %es:10(%di), %ecx
	add $2047, %ecx
	shr $11, %ecx
	mov $0x7E0, %ax
	mov %ax, %es
	xor %di, %di
	call ReadSectors
10:
	popa
	movl (ReturnValue), %ebx
	ret
11:
	mov $NotFoundMsg, %si
	call WriteString
	
	mov (FileName), %si
	call WriteString
	
	cmpb $0x01, (FolderFlag)
	je 12f
	
	mov $FileMsg, %si
	call WriteString
	jmp 13f
12:
	mov $FolderMsg, %si
	call WriteString
13:
	call Reboot

ReadSectors:
	pusha														// Save all the registers
	
	movb (BootDrive), %dl										// Restore the boot drive to DL
	lea DAPBuffer, %si											// Load DS:SI with DAP buffer address
	
	movl %ecx, 2(%si)											// Fill the DAP buffer
	movw %di, 4(%si)
	movw %es, 6(%si)
	movl %ebx, 8(%si)
	
	mov $0x4200, %ax											// (Try to) use the extended read sectors function
	int $0x13
	
	jc 1f														// Failed?
	
	popa														// No, so just return
	ret
1:
	mov $BiosErrMsg, %si										// Yes...
	call WriteString
	call Reboot

WriteString:
	pusha														// Save all the registers
	mov $0x0E, %ah												// 0x0E is the print character function
	xor %bx, %bx												// Both BL and BH should be 0
1:
	lodsb														// Get the next character
	or %al, %al													// Termination (0)?
	jz 2f														// Yes, return
	int $0x10													// No, write it
	jmp 1b
2:
	popa														// Restore all the registers
	ret

Reboot:
	mov $AnyKeyMsg, %si											// Print the "any key" message
	call WriteString
	xor %ah, %ah												// 0x00 is the wait keystroke function
	int $0x16
	mov $NewLineMsg, %si										// New line (for qemu, as the other emulators/virtual machines/real computers don't need this)
	call WriteString
	call WriteString
	int $0x19													// Restart
1:
	hlt
	jmp 1b

DAPBuffer:
	.byte 0x10													// DAP size
	.byte 0x00													// Reserved
	.word 0x01													// Sector count
	.word 0x00													// Destination offset
	.word 0x00													// Destination segment
	.quad 0x00													// Sector LBA

BootDrive: .byte 0

BiosErrMsg: .asciz "BIOS Error\r\n"
RootDirErrMsg: .asciz "Couldn't find the root directory\r\n"
NotFoundMsg: .asciz "Couldn't find the "
FolderMsg: .asciz " folder\r\n"
FileMsg: .asciz " file\r\n"
BootFolder: .asciz "Boot"
BootmgrFile: .asciz "bootmgr.bin"
AnyKeyMsg: .asciz "Press any key to restart"
NewLineMsg: .byte 0x0D, 0x0A, 0
