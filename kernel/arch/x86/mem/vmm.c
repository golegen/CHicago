// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 19:19 BRT
// Last edited on June 29 of 2018, at 18:09 BRT

#include <chicago/arch/pmm.h>
#include <chicago/arch/vmm.h>

#include <chicago/mm.h>

UIntPtr MmKernelDirectory = 0;

PUInt32 MmCurrentDirectory = (PUInt32)0xFFFFF000;
PUInt32 MmCurrentTables = (PUInt32)0xFFC00000;

PUInt32 MmTempAddress = (PUInt32)0xFF800000;

UIntPtr MmGetPhysInt(UIntPtr dir, UIntPtr virt) {
	if (dir == 0) {																											// Pointer to the page dir is Null?
		return (UIntPtr)-1;																									// Yes, so we can't do anything
	} else if ((MmGetPDE(dir, virt) & 0x01) != 0x01) {																		// The page table exists?
		return (UIntPtr)-1;																									// No, so just return 0
	}
	
	MmMap((UIntPtr)MmTempAddress, MmGetPDE(dir, virt) & 0xFFFFF000, MM_MAP_KDEF);											// Map the page table to the temp addr
	
	UIntPtr ret = (MmGetPTEInt(MmTempAddress, virt) & 0xFFFFF000) + (virt & 0xFFF);											// Get what we want
	
	MmUnmap((UIntPtr)MmTempAddress);																						// And unmap the temp addr
	
	return ret;
}

UInt32 MmQueryInt(UIntPtr dir, UIntPtr virt) {
	if (dir == 0) {																											// Pointer to the page dir is Null?
		return (UInt32)-1;																									// Yes, so we can't do anything
	} else if ((MmGetPDE(dir, virt) & 0x01) != 0x01) {																		// The page table exists?
		return (UInt32)-1;																									// No, so just return 0
	}
	
	MmMap((UIntPtr)MmTempAddress, MmGetPDE(dir, virt) & 0xFFFFF000, MM_MAP_KDEF);											// Map the page table to the temp addr
	
	UInt32 page = MmGetPTEInt(MmTempAddress, virt);																			// Get what we want
	
	MmUnmap((UIntPtr)MmTempAddress);																							// And unmap it
	
	UInt32 ret = MM_MAP_READ;																								// Convert the page flags to MmMap flags
	
	if ((page & 0x02) == 0x02) {
		ret |= MM_MAP_WRITE;
	}
	
	if ((page & 0x04) == 0x04) {
		ret |= (MM_MAP_USER | MM_MAP_KERNEL);
	} else {
		ret |= MM_MAP_KERNEL;
	}
	
	return ret;
}

Boolean MmMapInt(UIntPtr dir, UIntPtr virt, UIntPtr phys, UInt32 flags) {
	if ((dir == 0) || ((virt % MM_PAGE_SIZE) != 0) || ((phys % MM_PAGE_SIZE) != 0)) {										// Basic sanity checks (check if it isn't a Null directory pointer and the check if the addresses are page aligned)
		return False;																										// Failed in any of them? Return False
	}
	
	UInt32 flags2 = 0x01;																									// Convert the MmMap flags to page flags
	
	if (((flags & MM_MAP_USER) == MM_MAP_USER) && ((flags & MM_MAP_KERNEL) == MM_MAP_KERNEL)) {
		flags2 |= 0x04;
	} else if ((flags & MM_MAP_USER) == MM_MAP_USER) {
		flags2 |= 0x04;
	}
	
	if ((flags & MM_MAP_WRITE) == MM_MAP_WRITE) {
		flags2 |= 0x02;
	}
	
	if ((MmGetPDE(dir, virt) & 0x01) != 0x01) {																				// This page table exists?
		UIntPtr block = MmAllocPage();																						// No, so let's alloc it
		
		if (block == 0) {																									// Failed?
			return False;																									// Then just return
		}
		
		if (virt >= 0xC0100000) {																							// Kernel-only page directory?
			MmSetPDE(dir, virt, block, 0x03);																				// Yes, so put the pde as present, writeable
		} else {
			MmSetPDE(dir, virt, block, 0x07);																				// No, so put the pde as present, writeable and set the user bit
		}
		
		MmMap((UIntPtr)MmTempAddress, block, MM_MAP_KDEF);																	// Map the page table to the temp addr
		
		for (UInt32 i = 0; i < 1024; i++) {																					// Clear the page table
			MmTempAddress[i] = 0;
		}
		
		MmSetPTEInt(MmTempAddress, virt, phys, flags2);																		// Map the phys addr to the virt addr
		MmUnmap((UIntPtr)MmTempAddress);																						// Unmap the temp addr
		
		return True;
	} else {
		MmMap((UIntPtr)MmTempAddress, MmGetPDE(dir, virt) & 0xFFFFF000, MM_MAP_KDEF);										// Yes, so map it to the temp addr
		MmSetPTEInt(MmTempAddress, virt, phys, flags2);																		// Map the phys addr to the virt addr
		MmUnmap((UIntPtr)MmTempAddress);																						// Unmap the temp addr
		
		return True;
	}
}

Boolean MmUnmapInt(UIntPtr dir, UIntPtr virt) {
	if ((dir == 0) || ((virt % MM_PAGE_SIZE) != 0)) {																		// Basic sanity checks (check if it isn't a Null directory pointer and the check if the address is page aligned)
		return False;																										// Failed in any of them? Return False
	} else if ((MmGetPDE(dir, virt) & 0x01) != 0x01) {																		// This page table exists?
		return False;																										// No, so return False
	} else {
		MmMap((UIntPtr)MmTempAddress, MmGetPDE(dir, virt) & 0xFFFFF000, MM_MAP_KDEF);										// Yes, so map it to the temp addr
		MmSetPTEInt(MmTempAddress, virt, 0, 0);																				// Unmap the virt addr
		MmUnmap((UIntPtr)MmTempAddress);																						// And unmap the temp addr
		
		return True;
	}
}

UIntPtr MmGetPhys(UIntPtr virt) {
	if ((MmGetPDE(MmCurrentDirectory, virt) & 0x01) != 0x01) {																// The page table exists?
		return 0;																											// No, so just return 0
	} else {
		return (MmGetPTE(MmCurrentTables, virt) & 0xFFFFF000) + (virt & 0xFFF);												// Yes!
	}
}

UInt32 MmQuery(UIntPtr virt) {
	if ((MmGetPDE(MmCurrentDirectory, virt) & 0x01) != 0x01) {																// The page table exists?
		return 0;																											// No, so just return 0
	}
	
	UInt32 page = MmGetPTE(MmCurrentTables, virt);																			// Let's get the pte
	UInt32 ret = MM_MAP_READ;																								// And convert the page flags to MmMap flags
	
	if ((page & 0x02) == 0x02) {
		ret |= MM_MAP_WRITE;
	}
	
	if ((page & 0x04) == 0x04) {
		ret |= (MM_MAP_USER | MM_MAP_KERNEL);
	} else {
		ret |= MM_MAP_KERNEL;
	}
	
	return ret;
}

Boolean MmMap(UIntPtr virt, UIntPtr phys, UInt32 flags) {
	if (((virt % MM_PAGE_SIZE) != 0) || ((phys % MM_PAGE_SIZE) != 0)) {														// Basic sanity checks (check if the addresses are page aligned)
		return False;																										// Failed in any of them? Return False
	}
	
	UInt32 flags2 = 0x01;																									// Convert the MmMap flags to page flags
	
	if (((flags & MM_MAP_USER) == MM_MAP_USER) && ((flags & MM_MAP_KERNEL) == MM_MAP_KERNEL)) {
		flags2 |= 0x04;
	} else if ((flags & MM_MAP_USER) == MM_MAP_USER) {
		flags2 |= 0x04;
	}
	
	if ((flags & MM_MAP_WRITE) == MM_MAP_WRITE) {
		flags2 |= 0x02;
	}
	
	if ((MmGetPDE(MmCurrentDirectory, virt) & 0x01) != 0x01) {																// This page table exists?
		UIntPtr block = MmAllocPage();																						// No, so let's alloc it
		
		if (block == 0) {																									// Failed?
			return False;																									// Then just return
		}
		
		if (virt >= 0xC0100000) {																							// Kernel-only page directory?
			MmSetPDE(MmCurrentDirectory, virt, block, 0x03);																// Yes, so put the pde as present, writeable
		} else {
			MmSetPDE(MmCurrentDirectory, virt, block, 0x07);																// No, so put the pde as present, writeable and set the user bit
		}
		
		Asm Volatile("invlpg (%0)" :: "b"(((UIntPtr)(&MmGetPTE(MmCurrentTables, virt))) & 0xFFFFF000) : "memory");			// Map it (already mapped but we need to update the pde)
		
		for (UInt32 i = 0; i < 1024; i++) {																					// Clear the page table
			((PUInt32)(((UIntPtr)(&MmGetPTE(MmCurrentTables, virt))) & 0xFFFFF000))[i] = 0;
		}
	}
	
	MmSetPTE(MmCurrentTables, virt, phys, flags2);																			// Map the phys addr to the virt addr
	
	return True;
}

Boolean MmUnmap(UIntPtr virt) {
	if ((virt % MM_PAGE_SIZE) != 0) {																						// Basic sanity check (check if the address is page aligned)
		return False;
	} else if ((MmGetPDE(MmCurrentDirectory, virt) & 0x01) != 0x01) {														// This page table exists?
		return False;																										// No, so return False
	} else {
		MmSetPTE(MmCurrentTables, virt, 0, 0);																				// Yes, so unmap the virt addr
		return True;
	}
}

Void MmSwitchDirectory(UIntPtr dir) {
	if (dir == 0) {																											// Null pointer?
		return;																												// Yes, so just return
	}
	
	UIntPtr phys = MmGetPhys((UIntPtr)dir);																					// Get the phys addr of the new page dir
	
	Asm Volatile("mov %0, %%cr3" :: "r"(phys));																				// And switch the CR3!
}

Void MmEnable(Void) {
	UInt32 cr0;
	Asm Volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80010000;																										// Set the paging bit and the write-protected bit
	Asm Volatile("mov %0, %%cr0" :: "r"(cr0));
}

Void MmDisable(Void) {
	UInt32 cr0;
	Asm Volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 &= ~0x80010000;																										// Unset the paging and the write-protected bit
	Asm Volatile("mov %0, %%cr0" :: "r"(cr0));
}

Void VMMPreInit(Void) {
	MmKernelDirectory = MmBootAlloc(1024 * sizeof(UInt32), True);															// Alloc the kernel directory, the MmInit is going to map everything we need!
}

Void VMMInit(Void) {
	for (UInt32 i = 0; i < 1024; i++) {
		((PUInt32)MmKernelDirectory)[i] = 0;
	}
	
	MmSetPDE(MmKernelDirectory, (UIntPtr)MmCurrentTables, MmGetPhys(MmKernelDirectory), 0x03);								// Setup the recursive mapping entry
	
	UIntPtr ksphys = ((UIntPtr)(&KernelStart)) - 0xC0000000;
	UIntPtr kephys = KernelRealEnd - 0xC0000000;
	
	for (UIntPtr i = ksphys; i < kephys && i < (ksphys + 0x400000); i += MM_PAGE_SIZE) {										// Map the kernel to 0xC0100000
		MmMapInt(MmKernelDirectory, 0xC0100000 + (i - ksphys), i, MM_MAP_KDEF | MM_MAP_EXEC);
	}
	
	MmSwitchDirectory(MmKernelDirectory);																					// Switch the page directory
	MmEnable();																												// And enable paging!
}
