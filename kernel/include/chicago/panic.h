// File author is Ítalo Lima Marconato Matias
//
// Created on June 30 of 2018, at 11:50 BRT
// Last edited on June 30 of 2018, at 11:51 BRT

#ifndef __CHICAGO_PANIC_H__
#define __CHICAGO_PANIC_H__

#include <chicago/types.h>

#define PANIC_MM_READWRITE_TO_NONPRESENT_AREA 0x00
#define PANIC_MM_WRITE_TO_READONLY_AREA 0x01
#define PANIC_KERNEL_UNEXPECTED_ERROR 0x02
#define PANIC_KERNEL_INIT_FAILED 0x03

Void Panic(UInt32 err);

#endif		// __CHICAGO_PANIC_H__
