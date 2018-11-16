// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 17:56 BRT
// Last edited on November 15 of 2018, at 17:58 BRT

#include <chicago/types.h>

UIntPtr MmAllocMemory(UIntPtr size) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x01), "b"(size));
	return ret;
}

Void MmFreeMemory(UIntPtr block) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x02), "b"(block));
}

UIntPtr MmReallocMemory(UIntPtr block, UIntPtr size) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x03), "b"(block), "c"(size));
	return ret;
}

UIntPtr MmGetUsage(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x04));
	return ret;
}
