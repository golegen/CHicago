// File author is Ítalo Lima Marconato Matias
//
// Created on June 24 of 2018, at 19:13 BRT
// Last edited on June 26 of 2018, at 15:30 BRT

#define __CHICAGO_VMM__

#include <chicago/arch/pmm.h>
#include <chicago/arch/pmm-int.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>

PUInt32 VMMKernelDirectory = Null;

PUInt32 VMMCurrentDirectory = (PUInt32)0xFFFFF000;
PUInt32 VMMCurrentTables = (PUInt32)0xFFC00000;

PUInt32 VMMTempAddress = (PUInt32)0xFF800000;

UInt32 VMMGetPhysInt(PUInt32 dir, UInt32 virt) {
	if (dir == Null) {																																							// Pointer to the page dir is Null?
		return (UInt32)-1;																																						// Yes, so we can't do anything
	} else if ((VMMGetPDE(dir, virt) & 0x01) != 0x01) {																															// The page table exists?
		return (UInt32)-1;																																						// No, so just return 0
	}
	
	VMMMap((UInt32)VMMTempAddress, VMMGetPDE(dir, virt) & 0xFFFFF000, VMM_MAP_KDEF);																							// Map the page table to the temp addr
	
	UInt32 ret = (VMMGetPTEInt(VMMTempAddress, virt) & 0xFFFFF000) + (virt & 0xFFF);																							// Get what we want
	
	VMMUnmap((UInt32)VMMTempAddress);																																			// And unmap the temp addr
	
	return ret;
}

UInt32 VMMQueryInt(PUInt32 dir, UInt32 virt) {
	if (dir == Null) {																																							// Pointer to the page dir is Null?
		return (UInt32)-1;																																						// Yes, so we can't do anything
	} else if ((VMMGetPDE(dir, virt) & 0x01) != 0x01) {																															// The page table exists?
		return (UInt32)-1;																																						// No, so just return 0
	}
	
	VMMMap((UInt32)VMMTempAddress, VMMGetPDE(dir, virt) & 0xFFFFF000, VMM_MAP_KDEF);																							// Map the page table to the temp addr
	
	UInt32 page = VMMGetPTEInt(VMMTempAddress, virt);																															// Get what we want
	
	VMMUnmap((UInt32)VMMTempAddress);																																			// And unmap it
	
	UInt32 ret = VMM_MAP_READ;																																					// Convert the page flags to VMMMap flags
	
	if ((page & 0x02) == 0x02) {
		ret |= VMM_MAP_WRITE;
	}
	
	if ((page & 0x04) == 0x04) {
		ret |= (VMM_MAP_USER | VMM_MAP_KERNEL);
	} else {
		ret |= VMM_MAP_KERNEL;
	}
	
	return ret;
}

Boolean VMMMapInt(PUInt32 dir, UInt32 virt, UInt32 phys, UInt32 flags) {
	if ((dir == Null) || ((virt % 0x1000) != 0) || ((phys % 0x1000) != 0)) {																									// Basic sanity checks (check if it isn't a Null directory pointer and the check if the addresses are page aligned)
		return False;																																							// Failed in any of them? Return False
	}
	
	UInt32 flags2 = 0x01;																																						// Convert the VMMMap flags to page flags
	
	if (((flags & VMM_MAP_USER) == VMM_MAP_USER) && ((flags & VMM_MAP_KERNEL) == VMM_MAP_KERNEL)) {
		flags2 |= 0x04;
	} else if ((flags & VMM_MAP_USER) == VMM_MAP_USER) {
		flags2 |= 0x04;
	}
	
	if ((flags & VMM_MAP_WRITE) == VMM_MAP_WRITE) {
		flags2 |= 0x02;
	}
	
	if ((VMMGetPDE(dir, virt) & 0x01) != 0x01) {																																// This page table exists?
		UInt32 block = PMMAllocFrame(1);																																		// No, so let's alloc it
		
		if (block == 0) {																																						// Failed?
			return False;																																						// Then just return
		}
		
		if (virt >= 0xC0100000) {																																				// Kernel-only page directory?
			VMMSetPDE(dir, virt, block, 0x03);																																	// Yes, so put the pde as present, writeable
		} else {
			VMMSetPDE(dir, virt, block, 0x07);																																	// No, so put the pde as present, writeable and set the user bit
		}
		
		VMMMap((UInt32)VMMTempAddress, block, VMM_MAP_KDEF);																													// Map the page table to the temp addr
		
		for (UInt32 i = 0; i < 1024; i++) {																																		// Clear the page table
			VMMTempAddress[i] = 0;
		}
		
		VMMSetPTEInt(VMMTempAddress, virt, phys, flags2);																														// Map the phys addr to the virt addr
		VMMUnmap((UInt32)VMMTempAddress);																																		// Unmap the temp addr
		
		return True;
	} else {
		VMMMap((UInt32)VMMTempAddress, VMMGetPDE(dir, virt) & 0xFFFFF000, VMM_MAP_KDEF);																						// Yes, so map it to the temp addr
		VMMSetPTEInt(VMMTempAddress, virt, phys, flags2);																														// Map the phys addr to the virt addr
		VMMUnmap((UInt32)VMMTempAddress);																																		// Unmap the temp addr
		
		return True;
	}
}

Boolean VMMUnmapInt(PUInt32 dir, UInt32 virt) {
	if ((dir == Null) || ((virt % 0x1000) != 0)) {																																// Basic sanity checks (check if it isn't a Null directory pointer and the check if the address is page aligned)
		return False;																																							// Failed in any of them? Return False
	} else if ((VMMGetPDE(dir, virt) & 0x01) != 0x01) {																															// This page table exists?
		return False;																																							// No, so return False
	} else {
		VMMMap((UInt32)VMMTempAddress, VMMGetPDE(dir, virt) & 0xFFFFF000, VMM_MAP_KDEF);																						// Yes, so map it to the temp addr
		VMMSetPTEInt(VMMTempAddress, virt, 0, 0);																																// Unmap the virt addr
		VMMUnmap((UInt32)VMMTempAddress);																																		// And unmap the temp addr
		
		return True;
	}
}

UInt32 VMMGetPhys(UInt32 virt) {
	if ((VMMGetPDE(VMMCurrentDirectory, virt) & 0x01) != 0x01) {																												// The page table exists?
		return 0;																																								// No, so just return 0
	} else {
		return (VMMGetPTE(VMMCurrentTables, virt) & 0xFFFFF000) + (virt & 0xFFF);																								// Yes!
	}
}

UInt32 VMMQuery(UInt32 virt) {
	if ((VMMGetPDE(VMMCurrentDirectory, virt) & 0x01) != 0x01) {																												// The page table exists?
		return 0;																																								// No, so just return 0
	}
	
	UInt32 page = VMMGetPTE(VMMCurrentTables, virt);																															// Let's get the pte
	UInt32 ret = VMM_MAP_READ;																																					// And convert the page flags to VMMMap flags
	
	if ((page & 0x02) == 0x02) {
		ret |= VMM_MAP_WRITE;
	}
	
	if ((page & 0x04) == 0x04) {
		ret |= (VMM_MAP_USER | VMM_MAP_KERNEL);
	} else {
		ret |= VMM_MAP_KERNEL;
	}
	
	return ret;
}

Boolean VMMMap(UInt32 virt, UInt32 phys, UInt32 flags) {
	if (((virt % 0x1000) != 0) || ((phys % 0x1000) != 0)) {																														// Basic sanity checks (check if the addresses are page aligned)
		return False;																																							// Failed in any of them? Return False
	}
	
	UInt32 flags2 = 0x01;																																						// Convert the VMMMap flags to page flags
	
	if (((flags & VMM_MAP_USER) == VMM_MAP_USER) && ((flags & VMM_MAP_KERNEL) == VMM_MAP_KERNEL)) {
		flags2 |= 0x04;
	} else if ((flags & VMM_MAP_USER) == VMM_MAP_USER) {
		flags2 |= 0x04;
	}
	
	if ((flags & VMM_MAP_WRITE) == VMM_MAP_WRITE) {
		flags2 |= 0x02;
	}
	
	if ((VMMGetPDE(VMMCurrentDirectory, virt) & 0x01) != 0x01) {																												// This page table exists?
		UInt32 block = PMMAllocFrame(1);																																		// No, so let's alloc it
		
		if (block == 0) {																																						// Failed?
			return False;																																						// Then just return
		}
		
		if (virt >= 0xC0100000) {																																				// Kernel-only page directory?
			VMMSetPDE(VMMCurrentDirectory, virt, block, 0x03);																													// Yes, so put the pde as present, writeable
		} else {
			VMMSetPDE(VMMCurrentDirectory, virt, block, 0x07);																													// No, so put the pde as present, writeable and set the user bit
		}
		
		Asm Volatile("invlpg (%0)" :: "b"(((UInt32)(&VMMGetPTE(VMMCurrentTables, virt))) & 0xFFFFF000) : "memory");																// Map it (already mapped but we need to update the pde)
		
		for (UInt32 i = 0; i < 1024; i++) {																																		// Clear the page table
			((PUInt32)(((UInt32)(&VMMGetPTE(VMMCurrentTables, virt))) & 0xFFFFF000))[i] = 0;
		}
	}
	
	VMMSetPTE(VMMCurrentTables, virt, phys, flags2);																															// Map the phys addr to the virt addr
	
	return True;
}

Boolean VMMUnmap(UInt32 virt) {
	if ((virt % 0x1000) != 0) {																																					// Basic sanity check (check if the address is page aligned)
		return False;
	} else if ((VMMGetPDE(VMMCurrentDirectory, virt) & 0x01) != 0x01) {																											// This page table exists?
		return False;																																							// No, so return False
	} else {
		VMMSetPTE(VMMCurrentTables, virt, 0, 0);																																	// Yes, so unmap the virt addr
		return True;
	}
}

Void VMMSwitchDirectory(PUInt32 dir) {
	if (dir == Null) {																																							// Null pointer?
		return;																																									// Yes, so just return
	}
	
	UInt32 phys = VMMGetPhys((UInt32)dir);																																		// Get the phys addr of the new page dir
	
	Asm Volatile("mov %0, %%cr3" :: "r"(phys));																																	// And switch the CR3!
}

Void VMMEnable(Void) {
	UInt32 cr0;
	Asm Volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80010000;																																							// Set the paging bit and the write-protected bit
	Asm Volatile("mov %0, %%cr0" :: "r"(cr0));
}

Void VMMDisable(Void) {
	UInt32 cr0;
	Asm Volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 &= ~0x80010000;																																							// Unset the paging and the write-protected bit
	Asm Volatile("mov %0, %%cr0" :: "r"(cr0));
}

Void VMMFixMultiboot(PMultibootHeader mb) {
	mb->boot_device += 0xC0000000;																																				// Fix the multiboot pointer structure
	mb->cmd_line += 0xC0000000;
	mb->mods_address += 0xC0000000;
	mb->address += 0xC0000000;
	mb->mmap_address += 0xC0000000;
	mb->drives_address += 0xC0000000;
	mb->config_table += 0xC0000000;
	mb->boot_loader_name += 0xC0000000;
	mb->apm_table += 0xC0000000;
	mb->vbe_control_info += 0xC0000000;
	mb->vbe_mode_info += 0xC0000000;
}

Void VMMPreInit(Void) {
	VMMKernelDirectory = (PUInt32)PMMDumbAlloc(1024 * sizeof(UInt32), True);																									// Alloc the kernel directory, the VMMInit is going to map everything we need!
}

Void VMMInit(Void) {
	for (UInt32 i = 0; i < 1024; i++) {
		VMMKernelDirectory[i] = 0;
	}
	
	VMMSetPDE(VMMKernelDirectory, (UInt32)VMMCurrentTables, VMMGetPhys((UInt32)VMMKernelDirectory), 0x03);																		// Setup the recursive mapping entry
	
	UInt32 ksphys = ((UInt32)(&KernelStart)) - 0xC0000000;
	UInt32 kephys = KernelRealEnd - 0xC0000000;
	
	for (UInt32 i = ksphys; i < kephys && i < (ksphys + 0x400000); i += 0x1000) {																								// Map the kernel to 0xC0100000
		VMMMapInt(VMMKernelDirectory, 0xC0100000 + (i - ksphys), i, VMM_MAP_KDEF | VMM_MAP_EXEC);
	}
	
	VMMSwitchDirectory(VMMKernelDirectory);																																		// Switch the page directory
	VMMEnable();																																								// And enable paging!
}
