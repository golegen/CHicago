; File author is Ítalo Lima Marconato Matias
;
; Created on February 23 of 2019, at 14:36 BRT
; Last edited on February 23 of 2019, at 15:45 BRT

global AppEntry
AppEntry:
	mov eax, ConsoleDeviceName					; First, let's open the console device
	call FsOpenFile
	
	mov ebx, 14									; Now, let's write "Hello, World!" to it
	mov ecx, Message
	call FsWriteFile
AppEntry.Loop:
	jmp AppEntry.Loop

section .data

ConsoleDeviceName: dd "\Devices\Console", 0
Message: dd "Hello, World!", 0
