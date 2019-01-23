// File author is Ítalo Lima Marconato Matias
//
// Created on December 21 of 2018, at 23:21 BRT
// Last edited on January 23 of 2019, at 16:14 BRT

.ifdef ARCH_64
.code64


JumpGDTStart:
	.int 0x00000000, 0x00000000
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x98, 0xCF, 0x00
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x00
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0x0F, 0x00
JumpGDTEnd:
JumpGDTPointer:
	.word JumpGDTEnd - JumpGDTStart - 1
	.int JumpGDTStart

JumpEAX: .int 0
JumpEBX: .int 0
JumpECX: .int 0
JumpEDX: .int 0
JumpESI: .int 0
.global ArchJumpInt
ArchJumpInt:
	mov %ecx, (JumpEAX)
	mov %edx, (JumpEBX)
	mov %r8, %rax
	mov %eax, (JumpECX)
	mov %r9, %rax
	mov %eax, (JumpEDX)
	mov 0x28(%rsp), %rax
	mov %rax, (JumpESI)
	
	cli
	push $0x10
	push $1f
	lretq
1:
.code32
	mov %cr0, %eax
	and $0x7FFEFFFF, %eax
	mov %eax, %cr0
	mov $0xC0000080, %ecx
	rdmsr
	and $0xFFFFFEFF, %eax
	wrmsr
	mov %cr4, %eax
	and $0xFFFFFFDF, %eax
	mov %eax, %cr4
	
	mov (JumpEAX), %eax
	mov (JumpEBX), %ebx
	mov (JumpECX), %ecx
	mov (JumpEDX), %edx
	mov (JumpESI), %esi
	
	jmp *%eax
.else
.code32

.global _ArchJumpInt
_ArchJumpInt:
	mov 20(%esp), %esi
	mov 16(%esp), %edx
	mov 12(%esp), %ecx
	mov 8(%esp), %ebx
	mov 4(%esp), %eax
	jmp *%eax
.endif
