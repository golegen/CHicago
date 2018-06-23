// File author is Ítalo Lima Marconato Matias
//
// Created on June 22 of 2018, at 19:13 BRT
// Last edited on June 22 of 2018, at 20:01 BRT

#define __CHICAGO_VMM__

#include <chicago/arch/pmm.h>
#include <chicago/arch/pmm-int.h>
#include <chicago/arch/vmm.h>

#include <chicago/alloc.h>

PUInt32 VMMKernelDirectory = Null;
PUInt32 VMMCurrentDirectory = Null;

UInt32 VMMGetPhysInt(PUInt32 dir, UInt32 virt) {
	if (dir == Null) {																																							// The initial (boot) page directory?
		if ((virt >= 0xC0000000) && (virt < 0xC0400000)) {																														// Yes, it's inside of the kernel region?
			return virt - 0xC0000000;																																			// Yes, just subtract 0xC0000000
		} else {
			return virt;																																						// No, just return it
		}
	}
	
	if (dir[virt >> 22] == 0) {																																					// The page table exists?
		return -1;																																								// Nope, return -1
	} else if ((virt >> 22) == 768) {																																			// 0xC0100000-0xC0400000?
		return (((PUInt32)(dir[1024]))[(virt >> 12) & 0x3FF] & ~0xFFF) + (virt & 0xFFF);																						// Yes, and we have a pre-allocated table for it!
	} else if ((virt >> 22) == 1023) {																																			// 0xFFC00000-the limit?
		return (((PUInt32)(dir[1025]))[(virt >> 12) & 0x3FF] & ~0xFFF) + (virt & 0xFFF);																						// Yes, same as above
	}
	
	VMMMap(0xFFC00000, dir[virt >> 22] & ~0xFFF, VMM_MAP_KDEFAULTS);																											// Map the page table to the temp addr
	
	UInt32 ret = (((PUInt32)0xFFC00000)[(virt >> 12) & 0x3FFF] & ~0xFFF) + (virt & 0xFFF);																						// Get what we want
	
	VMMUnmap(0xFFC00000);																																						// And unmap the temp addr
	
	return ret;
}

UInt32 VMMGetPhys(UInt32 virt) {
	return VMMGetPhysInt(VMMCurrentDirectory, virt);																															// Just call the internal function
}

Boolean VMMMapInt(PUInt32 dir, UInt32 virt, UInt32 phys, UInt32 flags) {
	if (dir == Null) {																																							// The initial (boot) page directory?
		return False;																																							// Yes, so we can't do anything
	} else if ((virt % 0x1000) != 0) {																																			// The virt is page aligned?
		return False;																																							// No
	} else if ((phys % 0x1000) != 0) {								 																											// The phys is page aligned?
		return False;																																							// No
	}
	
	Boolean usr = False;																																						// Let's convert our flags to page flags
	Boolean rw = False;
	
	if (((flags & VMM_MAP_USER) == VMM_MAP_USER) && ((flags & VMM_MAP_KERNEL) == VMM_MAP_KERNEL)) {
		usr = True;
	} else if ((flags & VMM_MAP_USER) == VMM_MAP_USER) {
		usr = True;
	} else if ((flags & VMM_MAP_KERNEL) == VMM_MAP_KERNEL) {
		usr = False;
	} else {
		usr = False;
	}
	
	if ((flags & VMM_MAP_WRITE) == VMM_MAP_WRITE) {
		rw = True;
	}
	
	if (dir[virt >> 22] == 0) {
		UInt32 block = PMMAllocFrame();																																			// No, so let's allocate it!
		
		if (block == 0) {																																						// No more memory?
			return False;																																						// So f*** everything and return
		}
		
		if (!VMMMap(0xFFC00000, block, VMM_MAP_KDEFAULTS)) {																													// Let's map this block to our special temp address
			PMMFreeFrame(block);																																				// FAILED? WTF?
			return False;
		}
		
		PUInt32 tab = (PUInt32)0xFFC00000;
		
		for (UInt32 i = 0; i < 1024; i++) {																																		// Clean it
			tab[i] = 0;
		}
		
		tab[(virt >> 12) & 0x3FF] = (phys & 0xFFFFF000) | (usr ? 0x04 : 0x00) | (rw ? 0x02 : 0x00) | 0x01;																		// And map what the user (or us) want
		
		VMMUnmap(0xFFC00000);																																					// Unmap the temp address
		
		dir[virt >> 22] = block | 0x03;																																			// The table should be rw and present
		
		return True;
	} else if ((virt >> 22) == 768) {																																			// 0xC0100000-0xC0400000?
		((PUInt32)(dir[1024]))[(virt >> 12) & 0x3FF] = (phys & 0xFFFFF000) | (usr ? 0x04 : 0x00) | (rw ? 0x02 : 0x00) | 0x01;													// Yes, and we have a pre-allocated table for it!
		return True;
	} else if ((virt >> 22) == 1023) {																																			// 0xFFC00000-the limit?
		((PUInt32)(dir[1025]))[(virt >> 12) & 0x3FF] = (phys & 0xFFFFF000) | (usr ? 0x04 : 0x00) | (rw ? 0x02 : 0x00) | 0x01;													// Yes, so it's the same as above
		return True;
	} else {
		if (!VMMMap(0xFFC00000, dir[virt >> 22] & ~0xFFF, VMM_MAP_KDEFAULTS)) {																									// Try to map the page table
			return False;																																						// Failed...
		}
		
		PUInt32 tab = (PUInt32)0xFFC00000;
		
		tab[(virt >> 12) & 0x3FF] = (phys & 0xFFFFF000) | (usr ? 0x04 : 0x00) | (rw ? 0x02 : 0x00) | 0x01;																		// And map what the user (or us) want
		
		VMMUnmap(0xFFC00000);																																					// Unmap the temp addr
		
		return True;
	}
}

Boolean VMMMap(UInt32 virt, UInt32 phys, UInt32 flags) {
	if (VMMMapInt(VMMCurrentDirectory, virt, phys, flags)) {																													// Just call the internal function...
		Asm Volatile("invlpg (%0)" :: "b"(virt) : "memory");																													// But this is the pos-paging function, so also flush the tlb!
		return True;
	} else {
		return False;
	}
}

Boolean VMMUnmapInt(PUInt32 dir, UInt32 virt) {
	if (dir == Null) {																																							// The initial (boot) page directory?
		return False;																																							// Yes, so we can't do anything
	} else if ((virt % 0x1000) != 0) {																																			// Virt is page aligned?
		return False;																																							// No
	}
	
	if (dir[virt >> 22] == 0) {																																					// The page table exists?
		return False;																																							// No, so just return
	} else if ((virt >> 22) == 768) {																																			// 0xC0100000-0xC0400000?
		((PUInt32)(dir[1024]))[(virt >> 12) & 0x3FF] = 0;																														// Yes, and we have a pre-allocated table for it!
		return True;
	} else if ((virt >> 22) == 1023) {																																			// 0xFFC00000-the limit?
		((PUInt32)(dir[1025]))[(virt >> 12) & 0x3FF] = 0;																														// Tes, so it's the same as above
		return True;
	} else {
		if (!VMMMap(0xFFC00000, dir[virt >> 22] & ~0xFFF, VMM_MAP_KDEFAULTS)) {																									// Try to map the page table
			return False;																																						// Failed...
		}
		
		PUInt32 tab = (PUInt32)0xFFC00000;
		
		tab[(virt >> 12) & 0x3FF] = 0;																																			// Unmap what the user (or us) want
		
		VMMUnmap(0xFFC00000);																																					// And unmap the temp addr
		
		return True;
	}
}

Boolean VMMUnmap(UInt32 virt) {
	if (VMMUnmapInt(VMMCurrentDirectory, virt)) {																																// Just call the internal function...
		Asm Volatile("invlpg (%0)" :: "b"(virt) : "memory");																													// But this is the pos-paging function, so also flush the tlb!
		return True;
	} else {
		return False;
	}
}

PUInt32 VMMCreateDirectory(Boolean preinit) {
	PUInt32 dir = Null; 
	PUInt32 krnl = Null;
	PUInt32 temp = Null;
	
	if (preinit) {																																								// Use PMMDumbAlloc?
		dir = (PUInt32)PMMDumbAlloc(1027 * sizeof(UInt32), True);																												// Yes, so alloc the page directory
		krnl = (PUInt32)PMMDumbAlloc(1024 * sizeof(UInt32), True);																												// The initial kernel (0xC0100000-0xC0400000) page table
		temp = (PUInt32)PMMDumbAlloc(1024 * sizeof(UInt32), True);																												// And the temp (0xFFC00000-the limit) page table
	} else {
		dir = (PUInt32)MemoryAlignedAlloc(1026 * sizeof(UInt32), 0x1000);																										// No, so it's the same as above, but using MemoryAlignedAlloc
		krnl = (PUInt32)MemoryAlignedAlloc(1024 * sizeof(UInt32), 0x1000);
		temp = (PUInt32)MemoryAlignedAlloc(1024 * sizeof(UInt32), 0x1000);
	}
	
	if ((dir == Null) || (temp == Null) || (krnl == Null)) {																													// Failed?
		if ((!preinit) && (temp != Null)) {																																		// If we're not using the PMMDumbAlloc, so let's free what we were able to alloc
			MemoryAlignedFree((UInt32)temp);
		}
		
		if ((!preinit) && (krnl != Null)) {
			MemoryAlignedFree((UInt32)krnl);
		}
		
		if ((!preinit) && (dir != Null)) {
			MemoryAlignedFree((UInt32)dir);
		}
		
		return Null;
	}
	
	for (UInt32 i = 0; i < 1027; i++) {																																			// Clean everything!
		if (i < 1024) {
			dir[i] = 0;
			krnl[i] = 0;
			temp[i] = 0;
		} else {
			dir[i] = 0;
		}
	}
	
	dir[768] = VMMGetPhys(((UInt32)krnl)) | 0x03;																																// Setup everything
	dir[1023] = VMMGetPhys(((UInt32)temp)) | 0x03;
	dir[1024] = (UInt32)krnl;
	dir[1025] = (UInt32)temp;
	dir[1026] = preinit;
	
	if (!preinit) {																																								// And (if we aren't in the preinit, map some regions)
		for (UInt32 i = (((UInt32)(&KernelStart)) - 0xC0000000); i < (KernelRealEnd - 0xC0000000) && i < ((((UInt32)(&KernelStart)) - 0xC0000000) + 0x400000); i += 0x1000) {	// Map the kernel region
			VMMMapInt(dir, 0xC0100000 + (i - (((UInt32)(&KernelStart)) - 0xC0000000)), i, VMM_MAP_KDEFAULTS | VMM_MAP_EXEC);
		}
		
		for (UInt32 i = (UInt32)dir; i < ((UInt32)dir) + (1027 * sizeof(UInt32)); i += 0x1000) {																				// Map the page directory itself
			VMMMapInt(dir, i, VMMGetPhys(i), VMM_MAP_KDEFAULTS | VMM_MAP_EXEC);
		}
		
		for (UInt32 i = (UInt32)krnl; i < ((UInt32)krnl) + (1027 * sizeof(UInt32)); i += 0x1000) {																				// Map the kernel page table
			VMMMapInt(dir, i, VMMGetPhys(i), VMM_MAP_KDEFAULTS | VMM_MAP_EXEC);
		}
		
		for (UInt32 i = (UInt32)temp; i < ((UInt32)temp) + (1027 * sizeof(UInt32)); i += 0x1000) {																				// And the temp page table
			VMMMapInt(dir, i, VMMGetPhys(i), VMM_MAP_KDEFAULTS | VMM_MAP_EXEC);
		}
	}
	
	return dir;
}

Void VMMFreeDirectory(PUInt32 dir) {
	if (dir == Null) {																																							// It's the initial one? (or just some null pointer)
		return;																																									// Yes, so we can't do anything
	} else if (dir[1026] == True) {																																				// It's the preinit, kernel one?
		return;																																									// No way to free it
	}
	
	for (UInt32 i = 0; i < 1024; i++) {																																			// Free the used frames (except for the 768th and the 1023th, we're going to free them later)
		if ((i != 768) && (i != 1023)) {
			if (dir[i] != 0) {
				PMMFreeFrame(dir[i] & ~0xFFF);
			}
		}
	}
	
	MemoryAlignedFree(dir[1024]);																																				// Free the static allocated page tables
	MemoryAlignedFree(dir[1025]);
	MemoryAlignedFree((UInt32)dir);																																				// And the page directory
}

Void VMMSwitchDirectory(PUInt32 dir) {
	if (dir == Null) {
		return;
	}
	
	VMMCurrentDirectory = dir;																																					// Set the current dir
	
	Asm Volatile("mov %0, %%cr3" :: "r"(VMMGetPhys((UInt32)dir)));																												// And switch the CR3!
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
	cr0 &= ~0x80010000;																																							// Unset the paging and the write-protected bit (i don't think this is a good idea, as we're an higher-half kernel...
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
	VMMKernelDirectory = VMMCreateDirectory(True);																																// Create the kernel directory, the VMMInit is going to map everything we need!
}

Void VMMInit(Void) {
	for (UInt32 i = (((UInt32)(&KernelStart)) - 0xC0000000); i < (KernelRealEnd - 0xC0000000) && i < ((((UInt32)(&KernelStart)) - 0xC0000000) + 0x400000); i += 0x1000) {
		VMMMapInt(VMMKernelDirectory, 0xC0100000 + (i - (((UInt32)(&KernelStart)) - 0xC0000000)), i, VMM_MAP_KDEFAULTS | VMM_MAP_EXEC);
	}
	
	VMMSwitchDirectory(VMMKernelDirectory);																																		// Switch the directory
	VMMEnable();																																								// And enable paging!
}
