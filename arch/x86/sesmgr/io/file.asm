; File author is Ítalo Lima Marconato Matias
;
; Created on February 23 of 2019, at 14:47 BRT
; Last edited on February 23 of 2019, at 14:47 BRT

global FsOpenFile
FsOpenFile:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x15
	int 0x3F
	ret

global FsCloseFile
FsCloseFile:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x16
	int 0x3F
	ret

global FsReadFile
FsReadFile:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x17
	int 0x3F
	ret

global FsWriteFile
FsWriteFile:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x18
	int 0x3F
	ret

global FsMountFile
FsMountFile:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x19
	int 0x3F
	ret

global FsUmountFile
FsUmountFile:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x1A
	int 0x3F
	ret

global FsReadDirectoryEntry
FsReadDirectoryEntry:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x1B
	int 0x3F
	ret

global FsFindInDirectory
FsFindInDirectory:
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x1C
	int 0x3F
	ret

global FsCreateFile
FsCreateFile:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x1D
	int 0x3F
	ret

global FsControlFile
FsControlFile:
	mov esi, edx					; ESI = Fourth argument
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x1E
	int 0x3F
	ret

global FsGetSize
FsGetSize:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x1F
	int 0x3F
	ret

global FsGetPosition
FsGetPosition:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x20
	int 0x3F
	ret

global FsSetPosition
FsSetPosition:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x21
	int 0x3F
	ret
