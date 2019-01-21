// File author is Ítalo Lima Marconato Matias
//
// Created on December 21 of 2018, at 23:21 BRT
// Last edited on January 21 of 2019, at 11:50 BRT

.ifdef ARCH_64
.code64

.global SubarchJumpInt
SubarchJumpInt:
	mov %rcx, %rax
	mov %rdx, %rbx
	mov %r8, %rcx
	mov %r9, %rdx
	mov 0x28(%rsp), %rsi
	jmp *%rax
.else
.code32

.global _SubarchJumpInt
_SubarchJumpInt:
	mov 20(%esp), %esi
	mov 16(%esp), %edx
	mov 12(%esp), %ecx
	mov 8(%esp), %ebx
	mov 4(%esp), %eax
	jmp *%eax
.endif
