; File author is Ítalo Lima Marconato Matias
;
; Created on February 23 of 2019, at 15:25 BRT
; Last edited on February 23 of 2019, at 15:25 BRT

global MmAllocMemory
MmAllocMemory:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x01
	int 0x3F
	ret

global MmFreeMemory
MmFreeMemory:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x02
	int 0x3F
	ret

global MmReallocMemory
MmReallocMemory:
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x03
	int 0x3F
	ret

global MmGetUsage
MmGetUsage:
	mov eax, 0x04
	int 0x3F
	ret
