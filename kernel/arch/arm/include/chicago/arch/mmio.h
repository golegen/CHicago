// File author is Ítalo Lima Marconato Matias
//
// Created on August 11 of 2018, at 16:39 BRT
// Last edited on August 11 of 2018, at 16:39 BRT

#ifndef __CHICAGO_ARCH_MMIO_H__
#define __CHICAGO_ARCH_MMIO_H__

#include <chicago/types.h>

static inline Void MMIOWrite(UIntPtr addr, UIntPtr data) {
	*(Volatile PUIntPtr)addr = data;
}

static inline UInt32 MMIORead(UIntPtr addr) {
	return *(Volatile PUIntPtr)addr;
}

static inline Void MMIODelay(UInt32 count) {
	Asm Volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n" : "=r"(count): [count]"0"(count) : "cc");
}

#endif		// __CHICAGO_ARCH_MMIO_H__
