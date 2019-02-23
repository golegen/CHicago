; File author is Ítalo Lima Marconato Matias
;
; Created on February 23 of 2019, at 15:28 BRT
; Last edited on February 23 of 2019, at 15:28 BRT

global IpcCreatePort
IpcCreatePort:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x22
	int 0x3F
	ret

global IpcRemovePort
IpcRemovePort:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x23
	int 0x3F
	ret

global IpcSendMessage
IpcSendMessage:
	mov esi, edx					; ESI = Fourth argument
	mov edx, ecx					; EDX = Third argument
	mov ecx, ebx					; ECX = Second argument
	mov ebx, eax					; EBX = First argument
	mov eax, 0x24
	int 0x3F
	ret

global IpcReceivePort
IpcReceivePort:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x25
	int 0x3F
	ret

