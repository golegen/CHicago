// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:44 BRT
// Last edited on January 21 of 2019, at 12:11 BRT

#define StrDuplicate Dummy

#include <efi.h>
#include <efilib.h>

#undef StrDuplicate

#include <chicago/string.h>

UIntPtr MemAllocate(UIntPtr size) {
	VOID *ptr;
	
	if (EFI_ERROR(BS->AllocatePool(PoolAllocationType, size, &ptr))) {															// Call the AllocatePool function
		ptr = Null;																												// Failed...
	}
	
	return (UIntPtr)ptr;																										// Return
}

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

Void MemFree(UIntPtr block) {
	BS->FreePool((VOID*)block);																									// Redirect to the FreePool function
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
