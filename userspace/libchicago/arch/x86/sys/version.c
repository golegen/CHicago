// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 18:07 BRT
// Last edited on November 15 of 2018, at 18:07 BRT

#include <chicago/version.h>

Void SysGetVersion(PSystemVersion ver) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x00), "b"((UIntPtr)ver));
}
