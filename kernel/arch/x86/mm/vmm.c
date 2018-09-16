// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 19:19 BRT
// Last edited on September 15 of 2018, at 19:27 BRT

#include <chicago/arch/pmm.h>
#include <chicago/arch/vmm.h>

#include <chicago/mm.h>
#include <chicago/string.h>

PUInt32 MmCurrentDirectory = (PUInt32)0xFFFFF000;
PUInt32 MmCurrentTables = (PUInt32)0xFFC00000;
UIntPtr MmTempAddressStart = 0xFF800000;
UIntPtr MmTempAddressEnd = 0xFFC00000;

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
	} else if ((MmGetPTE(MmCurrentTables, virt) & 0x01) != 0x01) {															// Same as above
		return 0;
	}
	
	UInt32 page = MmGetPTE(MmCurrentTables, virt);																			// Let's get the pte
	UInt32 ret = MM_MAP_READ | MM_MAP_EXEC;																					// And convert the page flags to MmMap flags
	
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

UIntPtr MmFindFreeVirt(UIntPtr start, UIntPtr end, UIntPtr count) {
	if (start % MM_PAGE_SIZE != 0) {																						// Page align the start
		start -= count % MM_PAGE_SIZE;
	}
	
	if (end % MM_PAGE_SIZE != 0) {																							// Page align the end
		end += MM_PAGE_SIZE - (end % MM_PAGE_SIZE);
	}
	
	if (count % MM_PAGE_SIZE != 0) {																						// Page align the count
		count += MM_PAGE_SIZE - (count % MM_PAGE_SIZE);
	}
	
	UIntPtr c = 0;
	UIntPtr p = start;
	
	for (UIntPtr i = start; i < end; i += MM_PAGE_SIZE * 1024) {															// Let's try to find the first free virtual address!
		if ((MmGetPDE(MmCurrentDirectory, i) & 0x01) == 0x01) {																// This PDE is allocated?
			for (UIntPtr j = 0; j < MM_PAGE_SIZE * 1024; j += MM_PAGE_SIZE) {												// Yes, let's check the PTEs
				if (((i == 0) && (j == 0)) || (MmGetPTE(MmCurrentTables, i + j) & 0x01) == 0x01) {							// Allocated?
					c = 0;																									// Yes :(
					p = i + (j + MM_PAGE_SIZE);
				} else {
					c += MM_PAGE_SIZE;																						// No! (+4KB)
					
					if (c >= count) {																						// We need more memory?
						return p;																							// No, so return!
					}
				}
			}
		} else {
			c += MM_PAGE_SIZE * 1024;																						// No! (+4MB)
			
			if (i < MM_PAGE_SIZE * 1024) {																					// 0x00000000?
				c -= MM_PAGE_SIZE;																							// Yes, but it's reserved...
				p += MM_PAGE_SIZE;
			}
			
			if (c >= count) {																								// We need more memory?
				return p;																									// No, so return!
			}
		}
	}
	
	return 0;																												// We failed :(
}

UIntPtr MmFindHighestFreeVirt(UIntPtr start, UIntPtr end, UIntPtr count) {
	if (start % MM_PAGE_SIZE != 0) {																						// Page align the start
		start -= count % MM_PAGE_SIZE;
	}
	
	if (end % MM_PAGE_SIZE != 0) {																							// Page align the end
		end += MM_PAGE_SIZE - (end % MM_PAGE_SIZE);
	}
	
	if (count % MM_PAGE_SIZE != 0) {																						// Page align the count
		count += MM_PAGE_SIZE - (count % MM_PAGE_SIZE);
	}
	
	UIntPtr c = 0;
	UIntPtr p = end;
	
	for (UIntPtr i = end - (MM_PAGE_SIZE * 1024); i > start; i -= MM_PAGE_SIZE * 1024) {									// Let's try to find the highest free virtual address!
		if ((MmGetPDE(MmCurrentDirectory, i) & 0x01) == 0x01) {																// This PDE is allocated?/
			for (UIntPtr j = MM_PAGE_SIZE * 1024; j > 0; j -= MM_PAGE_SIZE) {												// Yes, let's check the PTEs
				UIntPtr rj = j - 1;
				
				if ((i == start) && (rj == 0)) {																			// curr == start?
					return 0;																								// Yes, we failed :(
				} else if ((MmGetPTE(MmCurrentTables, i + rj) & 0x01) == 0x01) {											// Allocated?
					c = 0;																									// Yes :(
					p = (i + rj + 1) - MM_PAGE_SIZE;
				} else {
					c += MM_PAGE_SIZE;																						// No! (+4KB)
					
					if (c >= count) {																						// We need more memory?
						return p - count;																					// No, so return!
					}
				}
			}
		} else {
			c += MM_PAGE_SIZE * 1024;																						// No! (+4MB)
			
			if (i == 0) {																									// 0x00000000?
				c -= MM_PAGE_SIZE;																							// Yes...
			}
			
			if (c >= count) {																								// We need more memory?
				return p - count;																							// No, so return!
			}
		}
	}
	
	return 0;																												// We failed :(
}

UIntPtr MmMapTemp(UIntPtr phys, UInt32 flags) {
	if ((phys % MM_PAGE_SIZE) != 0) {																						// Align to page size
		phys -= phys % MM_PAGE_SIZE;
	}
	
	for (UIntPtr i = MmTempAddressStart; i < MmTempAddressEnd; i += MM_PAGE_SIZE) {											// Let's try to find an free temp address
		if (MmQuery(i) == 0) {																								// Free?
			if (MmMap(i, phys, flags)) {																					// Yes, so try to map it
				return i;																									// Mapped! Now we only need to return it
			}
		}
	}
	
	return 0;																												// Failed...
}

Boolean MmMap(UIntPtr virt, UIntPtr phys, UInt32 flags) {
	if ((virt % MM_PAGE_SIZE) != 0) {																						// Align to page size
		virt -= virt % MM_PAGE_SIZE;
	}
	
	if ((phys % MM_PAGE_SIZE) != 0) {																						// Align to page size
		phys -= phys % MM_PAGE_SIZE;
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
		UIntPtr block = MmReferencePage(0);																					// No, so let's alloc it
		
		if (block == 0) {																									// Failed?
			return False;																									// Then just return
		}
		
		if (virt >= 0xC0100000) {																							// Kernel-only page directory?
			MmSetPDE(MmCurrentDirectory, virt, block, 0x03);																// Yes, so put the pde as present, writeable
		} else {
			MmSetPDE(MmCurrentDirectory, virt, block, 0x07);																// No, so put the pde as present, writeable and set the user bit
		}
		
		Asm Volatile("invlpg (%0)" :: "b"(((UIntPtr)(&MmGetPTE(MmCurrentTables, virt))) & 0xFFFFF000) : "memory");			// Map it (already mapped but we need to update the pde)
		StrSetMemory((PVoid)(((UIntPtr)(&MmGetPTE(MmCurrentTables, virt))) & 0xFFFFF000), 0, 4096);							// Clear the page table
	}
	
	MmSetPTE(MmCurrentTables, virt, phys, flags2);																			// Map the phys addr to the virt addr
	
	return True;
}

Boolean MmUnmap(UIntPtr virt) {
	if ((MmGetPDE(MmCurrentDirectory, virt) & 0x01) != 0x01) {																// This page table exists?
		return False;																										// No, so return False
	} else if ((MmGetPTE(MmCurrentTables, virt) & 0x01) != 0x01) {															// Same as above
		return False;																										// No, so return False
	} else {
		MmSetPTE(MmCurrentTables, virt, 0, 0);																				// Yes, so unmap the virt addr
		return True;
	}
}

UIntPtr MmCreateDirectory(Void) {
	UIntPtr ret = MmReferencePage(0);																						// Allocate one physical page
	PUInt32 dir = Null;
	
	if (ret == 0) {																											// Failed?
		return 0;																											// Yes...
	}
	
	if ((dir = (PUInt32)MmMapTemp(ret, MM_MAP_KDEF)) == 0) {																// Try to map it to an temp addr
		MmDereferencePage(ret);																								// Failed...
		return 0;
	}
	
	for (UInt32 i = 0; i < 1024; i++) {																						// Let's fill the table!
		if (((MmCurrentDirectory[i] & 0x01) != 0x01) || (i < 768) || (i == 1022)) {											// Non-present, user or temp?
			dir[i] = 0;																										// Yes, so just zero it
		} else if (i == (((UIntPtr)MmCurrentTables) >> 22)) {																// Recursive mapping entry?
			dir[i] = (ret & 0xFFFFF000) | 3;																				// Yes
		} else {																											// Normal kernel entry?
			dir[i] = MmCurrentDirectory[i];																					// Yes
		}
	}
	
	MmUnmap((UIntPtr)dir);																									// Unmap the temp addr
	
	return ret;
}

UIntPtr MmCloneDirectory(Void) {
	UIntPtr ret = MmReferencePage(0);																						// Allocate one physical page
	PUInt32 dir = Null;
	
	if (ret == 0) {																											// Failed?
		return 0;																											// Yes...
	}
	
	if ((dir = (PUInt32)MmMapTemp(ret, MM_MAP_KDEF)) == 0) {																// Try to map it to an temp addr
		MmDereferencePage(ret);																								// Failed...
		return 0;
	}
	
	for (UInt32 i = 0; i < 1024; i++) {																						// Let's fill the table!
		if (((MmCurrentDirectory[i] & 0x01) != 0x01) || (i == 1022)) {														// Non-present or temp?
			dir[i] = 0;																										// Yes, so just zero it
		} else if (i < 768) {																								// User?
			UIntPtr tabpa = MmReferencePage(0);																				// Yes, allocate new physical page
			PUInt32 tabta = Null;
			
			if (tabpa == 0) {																								// Failed?
				for (UInt32 j = 0; j < i; j++) {																			// Yes... so let's undo everything...
					if ((dir[j] & 0x01) == 0x01) {																			// Present?
						PUInt32 tmp = (PUInt32)MmMapTemp(dir[j] & 0xFFFFF000, MM_MAP_KDEF);									// Yes, try to map it to an temp addr
						
						if (tmp != 0) {																						// Failed?
							for (UInt32 k = 0; k < 1024; k++) {																// No, so let's free (or just decrease the reference count to) all the used physical pages
								UIntPtr addr = (j * (1024 * 1024 * 4)) + (k * MM_PAGE_SIZE);								// Get the virt addr from j and k
								UIntPtr page = MmGetPTE(MmCurrentTables, addr);
								
								if (((tmp[k] & 0x01) == 0x01) && ((tmp[k] & 0x200) == 0x200)) {								// CoW?
									MmSetPTE(MmCurrentTables, addr, page & 0xFFFFF000, (page & 0xFFF) & ~0x200);			// Yes, so undo it in the current directory
									Asm Volatile("invlpg (%0)" :: "b"(addr) : "memory");
									MmDereferencePage(page & 0xFFFFF000);													// And use the dereference function
								} else if ((tmp[k] & 0x01) == 0x01) {														// Present?
									MmDereferencePage(page & 0xFFFFF000);													// Yes, just use the dereference function
								}
							}
							
							MmUnmap((UIntPtr)tmp);																			// Unmap the temp addr
						}
						
						MmDereferencePage(dir[j] & 0xFFFFF000);																// Free/dereference the physical address of the page table
					}
				}
				
				MmUnmap((UIntPtr)dir);																						// Unmap the dir temp addr
				MmDereferencePage(ret);																						// Free/dereference it
				
				return 0;
			} else if ((tabta = (PUInt32)MmMapTemp(tabpa, MM_MAP_KDEF)) == 0) {												// Let's try to map the page table to an temp addr
				MmDereferencePage(tabpa);																					// Failed, so let's undo everything
				
				for (UInt32 j = 0; j < i; j++) {
					if ((dir[j] & 0x01) == 0x01) {																			// Present?
						PUInt32 tmp = (PUInt32)MmMapTemp(dir[j] & 0xFFFFF000, MM_MAP_KDEF);									// Yes, try to map it to an temp addr
						
						if (tmp != 0) {																						// Failed?
							for (UInt32 k = 0; k < 1024; k++) {																// No, so let's free (or just decrease the reference count to) all the used physical pages
								UIntPtr addr = (j * (1024 * 1024 * 4)) + (k * MM_PAGE_SIZE);								// Get the virt addr using j and k
								UIntPtr page = MmGetPTE(MmCurrentTables, addr);
								
								if (((tmp[k] & 0x01) == 0x01) && ((tmp[k] & 0x200) == 0x200)) {								// CoW?
									if (MmGetReferences(page & 0xFFFFF000) == 2) {											// Yes, undo in the current directory?
										MmSetPTE(MmCurrentTables, addr, page & 0xFFFFF000, (page & 0xFFF) & ~0x200);		// Yes
										Asm Volatile("invlpg (%0)" :: "b"(addr) : "memory");
									}
									MmDereferencePage(page & 0xFFFFF000);													// Use the dereference function
								} else if ((tmp[k] & 0x01) == 0x01) {														// Present?
									MmDereferencePage(page & 0xFFFFF000);													// Yes, just use the dereference function
								}
							}
							
							MmUnmap((UIntPtr)tmp);																			// Unmap the temp addr
						}
						
						MmDereferencePage(dir[j] & 0xFFFFF000);																// Free/dereference the physical address of the page table
					}
				}
				
				MmUnmap((UIntPtr)dir);																						// Unmap the dir temp addr
				MmDereferencePage(ret);																						// Free/dereference it
				
				return 0;
			}
			
			dir[i] = (tabpa & 0xFFFFF000) | (MmCurrentDirectory[i] & 0xFFF);												// Set the new entry in the new page directory
			
			for (UInt32 j = 0; j < 1024; j++) {																				// And let's clone the table
				UIntPtr addr = (i * (1024 * 1024 * 4)) + (j * MM_PAGE_SIZE);												// Get the virt addr using i and j
				UIntPtr page = MmGetPTE(MmCurrentTables, addr);
				
				if ((page & 0x01) != 0x01) {																				// Non-present?
					tabta[j] = 0;																							// Yes, so just zero it
				} else if ((page & 0x02) == 0x02) {																			// Read/Write?
					MmReferencePage(page & 0xFFFFF000);																		// Yes, for read/write we're going to use CoW (Copy-on-Write), our page fault handler will handle it for us!
					MmSetPTEInt(tabta, addr, page & 0xFFFFF000, ((page & 0xFFF) & ~0x02) | 0x200);
					MmSetPTE(MmCurrentTables, addr, page & 0xFFFFF000, ((page & 0xFFF) & ~0x02) | 0x200);
					Asm Volatile("invlpg (%0)" :: "b"(addr) : "memory");
				} else {
					MmReferencePage(page & 0xFFFFF000);																		// Read only, we don't need CoW!
					MmSetPTEInt(tabta, addr, page & 0xFFFFF000, page & 0xFFF);
				}
			}
			
			MmUnmap((UIntPtr)tabta);																						// Unmap the temp addr
		} else if (i == (((UIntPtr)MmCurrentTables) >> 22)) {																// Recursive mapping entry?
			dir[i] = (ret & 0xFFFFF000) | 3;																				// Yes
		} else {																											// Normal kernel entry?
			dir[i] = MmCurrentDirectory[i];																					// Yes
		}
	}
	
	MmUnmap((UIntPtr)dir);																									// Unmap the temp addr
	
	return ret;
}

Void MmFreeDirectory(UIntPtr dir) {
	if (dir == 0) {																											// Sanity check 1
		return;
	} else if (dir == MmGetCurrentDirectory()) {																			// Sanity check 2 (we can't free ourself)
		return;
	}
	
	PUInt32 tmp = Null;
	
	if ((tmp = (PUInt32)MmMapTemp(dir, MM_MAP_KDEF)) == 0) {																// Let's try to map us to an temp addr
		MmDereferencePage(dir);																								// Failed, so just free the dir physical address
		return;
	}
	
	for (UInt32 i = 0; i < 768; i++) {																						// Let's free the user tables
		if ((tmp[i] & 0x01) == 0x01) {																						// Present?
			UIntPtr tabpa = tmp[i] & 0xFFFFF000;																			// Yes
			PUInt32 tabta = Null;
			
			if ((tabta = (PUInt32)MmMapTemp(tabpa, MM_MAP_KDEF)) == 0) {
				MmDereferencePage(tabpa);
				continue;
			}
			
			for (UInt32 j = 0; j < 1024; j++) {
				UIntPtr addr = (i * (1024 * 1024 * 4)) + (j * MM_PAGE_SIZE);
				UIntPtr page = MmGetPTEInt(tabta, addr);
				
				if (((page & 0x01) == 0x01) && ((page & 0x200) == 0x200)) {													// CoW?
					if (MmGetReferences(page & 0xFFFFF000) == 2) {															// Undo in the current directory?
						MmSetPTE(MmCurrentTables, addr, page & 0xFFFFF000, (page & 0xFFF) & ~0x200);						// Yes
						Asm Volatile("invlpg (%0)" :: "b"(addr) : "memory");
					}
					
					MmDereferencePage(page & 0xFFFFF000);																	// Use the dereference function
				} else if ((page & 0x01) == 0x01) {																			// Present?
					MmDereferencePage(page & 0xFFFFF000);																	// Yes, just use the dereference function
				}				
			}
			
			MmUnmap((UIntPtr)tabta);																						// Unmap the temp addr
			MmDereferencePage(tabpa);																						// And use the dereference function
		}
	}
}

UIntPtr MmGetCurrentDirectory(Void) {
	UIntPtr ret;
	Asm Volatile("mov %%cr3, %0" : "=r"(ret));																				// Current page directory (physical address) is in CR3
	return ret;
}

Void MmSwitchDirectory(UIntPtr dir) {
	if (dir == 0) {																											// Null pointer?
		return;																												// Yes, so just return
	}
	
	Asm Volatile("mov %0, %%cr3" :: "r"(dir));																				// Switch the CR3!
}
