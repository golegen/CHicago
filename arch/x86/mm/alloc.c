// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:44 BRT
// Last edited on December 19 of 2018, at 12:16 BRT

#include <chicago/alloc.h>
#include <chicago/string.h>

UIntPtr MemZAllocate(UIntPtr size) {
	UIntPtr ret = MemAllocate(size);																							// Allocate the memory
	
	if (ret) {
		StrSetMemory((PVoid)ret, 0, size);																						// And zero it
	}
	
	return ret;
}
