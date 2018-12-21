// File author is Ítalo Lima Marconato Matias
//
// Created on December 20 of 2018, at 11:28 BRT
// Last edited on December 20 of 2018, at 11:40 BRT

#include <efi.h>
#include <efilib.h>

#include <chicago/types.h>

UIntPtr MemAllocate(UIntPtr size) {
	VOID *ptr;
	
	if (EFI_ERROR(BS->AllocatePool(PoolAllocationType, size, &ptr))) {				// Call the AllocatePool function
		ptr = Null;																	// Failed...
	}
	
	return (UIntPtr)ptr;															// Return
}

Void MemFree(UIntPtr block) {
	BS->FreePool((VOID*)block);														// Redirect to the FreePool function
}
