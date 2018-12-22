// File author is Ítalo Lima Marconato Matias
//
// Created on December 21 of 2018, at 23:21 BRT
// Last edited on December 22 of 2018, at 01:17 BRT

.code32

.ifdef SUBARCH_EFI
.global _SubarchJumpInt
_SubarchJumpInt:
.else
.global SubarchJumpInt
SubarchJumpInt:
.endif
	mov 20(%esp), %esi
	mov 16(%esp), %edx
	mov 12(%esp), %ecx
	mov 8(%esp), %ebx
	mov 4(%esp), %eax
	jmp *%eax
