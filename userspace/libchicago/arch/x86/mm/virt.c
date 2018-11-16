// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 17:52 BRT
// Last edited on November 15 of 2018, at 17:56 BRT

#include <chicago/types.h>

UIntPtr VirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x05), "b"(addr), "c"(size), "d"(flags));
	return ret;
}

Boolean VirtFreeAddress(UIntPtr addr, UIntPtr size) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x06), "b"(addr), "c"(size));
	return ret;
}

UInt32 VirtQueryProtection(UIntPtr addr) {
	UInt32 ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x07), "b"(addr));
	return ret;
}

Boolean VirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x08), "b"(addr), "c"(size), "d"(flags));
	return ret;
}

UIntPtr VirtGetUsage(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x09));
	return ret;
}
