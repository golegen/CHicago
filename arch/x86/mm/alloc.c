// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:44 BRT
// Last edited on January 19 of 2019, at 14:00 BRT

#include <chicago/alloc.h>
#include <chicago/string.h>

UIntPtr MemAAllocate(UIntPtr size, UIntPtr align) {
	if (size == 0) {
		return 0;
	} else if (align == 0) {
		return 0;
	} else if ((align & (align - 1)) != 0) {
		return 0;
	}
	
	UIntPtr p1 = 0;
	PUIntPtr p2 = Null;
	Int off = align - 1 + sizeof(UIntPtr);
	
	if ((p1 = MemAllocate(size + off)) == 0) {
		return 0;
	}
	
	p2 = (PUIntPtr)((p1 + off) & ~(align - 1));
	p2[-1] = p1;
	
	return (UIntPtr)p2;
}

Void MemAFree(UIntPtr block) {
	MemFree(((PUIntPtr)block)[-1]);
}

UIntPtr MemZAllocate(UIntPtr size) {
	UIntPtr ret = MemAllocate(size);																							// Allocate the memory
	
	if (ret) {
		StrSetMemory((PVoid)ret, 0, size);																						// And zero it
	}
	
	return ret;
}
