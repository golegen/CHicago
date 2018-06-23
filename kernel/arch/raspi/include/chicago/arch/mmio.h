// File author is Ítalo Lima Marconato Matias
//
// Created on May 12 of 2018, at 00:15 BRT
// Last edited on May 12 of 2018, at 00:21 BRT

#ifndef __CHICAGO_ARCH_MMIO_H__
#define __CHICAGO_ARCH_MMIO_H__

#include <chicago/types.h>

static inline Void MMIOWrite(UInt32 addr, UInt32 data) {
	*(Volatile PUInt32)addr = data;
}

static inline UInt32 MMIORead(UInt32 addr) {
	return *(Volatile PUInt32)addr;
}

static inline Void MMIODelay(UInt32 count) {
	Asm Volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n" : "=r"(count): [count]"0"(count) : "cc");
}

#endif		// __CHICAGO_ARCH_MMIO_H__
