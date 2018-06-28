// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 18:42 BRT
// Last edited on June 28 of 2018, at 19:01 BRT

#include <chicago/mm.h>

PUIntPtr MmPageStack = Null;
IntPtr MmPageStackPointer = -1;
UIntPtr MmMaxPages = 0;
UIntPtr MmUsedPages = 0;

UIntPtr MmAllocPage(Void) {
	if ((MmPageStack == Null) || (MmPageStackPointer == -1)) {															// Sanity checks
		return 0;
	}
	
	MmUsedPages++;																										// Increase the used pages count
	
	return MmPageStack[MmPageStackPointer--];																			// It's just an stack, pop the top element
}

Void MmFreePage(UIntPtr addr) {
	if ((MmPageStack == Null) || (addr == 0)) {																			// Again, sanity checks
		return;
	}
	
	MmUsedPages--;																										// This time decrease the used pages count
	MmPageStack[++MmPageStackPointer] = addr;																			// And push this address to the top of the stack!
}

UIntPtr MmGetSize(Void) {
	return MmMaxPages * MM_PAGE_SIZE;
}

UIntPtr MmGetUsage(Void) {
	return MmUsedPages * MM_PAGE_SIZE;
}

UIntPtr MmGetFree(Void) {
	return (MmMaxPages - MmUsedPages) * MM_PAGE_SIZE;
}
