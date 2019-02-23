; File author is Ítalo Lima Marconato Matias
;
; Created on February 23 of 2019, at 15:22 BRT
; Last edited on February 23 of 2019, at 15:23 BRT

global VirtAllocAddress
VirtAllocAddress:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x05
	int 0x3F
	ret

global VirtFreeAddress
VirtFreeAddress:
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x06
	int 0x3F
	ret

global VirtQueryProtection
VirtQueryProtection:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x07
	int 0x3F
	ret

global VirtChangeProtection
VirtChangeProtection:
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x08
	int 0x3F
	ret

global VirtGetUsage
VirtGetUsage:
	mov eax, 0x09
	int 0x3F
	ret
