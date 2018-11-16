// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 17:59 BRT
// Last edited on November 16 of 2018, at 16:27 BRT

#include <chicago/process.h>

UIntPtr PsCreateThread(UIntPtr entry) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0A), "b"(entry));
	return ret;
}

UIntPtr PsGetTID(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0B));
	return ret;
}

UIntPtr PsGetPID(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0C));
	return ret;
}

Void PsSleep(UIntPtr ms) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0D), "b"(ms));
}

Void PsWaitThread(UIntPtr tid) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0E), "b"(tid));
}

Void PsWaitProcess(UIntPtr pid) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0F), "b"(pid));
}

Void PsLock(PLock lock) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x10), "b"((UIntPtr)lock));
}

Void PsUnlock(PLock lock) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x11), "b"((UIntPtr)lock));
}

Void PsExitThread(Void) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x12));
}

Void PsExitProcess(Void) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x13));
}

Void PsForceSwitch(Void) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x14));
}
