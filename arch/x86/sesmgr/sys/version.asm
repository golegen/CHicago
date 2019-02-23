; File author is Ítalo Lima Marconato Matias
;
; Created on February 23 of 2019, at 15:29 BRT
; Last edited on February 23 of 2019, at 15:29 BRT

global SysGetVersion
SysGetVersion:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x00
	int 0x3F
	ret
