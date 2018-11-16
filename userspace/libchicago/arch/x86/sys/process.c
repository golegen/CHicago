// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 17:59 BRT
// Last edited on November 15 of 2018, at 17:59 BRT

#include <chicago/process.h>

UIntPtr PsGetPID(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0A));
	return ret;
}

Void PsSleep(UIntPtr ms) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0B), "b"(ms));
}

Void PsWaitProcess(UIntPtr pid) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0C), "b"(pid));
}

Void PsLock(PLock lock) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0D), "b"((UIntPtr)lock));
}

Void PsUnlock(PLock lock) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0E), "b"((UIntPtr)lock));
}

Void PsExitProcess(Void) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0F));
}

Void PsForceSwitch(Void) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x10));
}
