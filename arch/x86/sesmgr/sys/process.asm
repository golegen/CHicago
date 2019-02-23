; File author is Ítalo Lima Marconato Matias
;
; Created on February 23 of 2019, at 14:53 BRT
; Last edited on February 23 of 2019, at 14:53 BRT

global PsCreateThread
PsCreateThread:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x0A
	int 0x3F
	ret

global PsGetTID
PsGetTID:
	mov eax, 0x0B
	int 0x3F
	ret

global PsGetPID
PsGetPID:
	mov eax, 0x0C
	int 0x3F
	ret

global PsSleep
PsSleep:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x0D
	int 0x3F
	ret

global PsWaitThread
PsWaitThread:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x0E
	int 0x3F
	ret

global PsWaitProcess
PsWaitProcess:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x0F
	int 0x3F
	ret

global PsLock
PsLock:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x10
	int 0x3F
	ret


global PsUnlock
PsUnlock:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x11
	int 0x3F
	ret

global PsExitThread
PsExitThread:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x12
	int 0x3F
	ret

global PsExitProcess
PsExitProcess:
	mov ebx, eax					; EBX = First argument
	mov eax, 0x13
	int 0x3F
	ret

global PsForceSwitch
PsForceSwitch:
	mov eax, 0x14
	int 0x3F
	ret
