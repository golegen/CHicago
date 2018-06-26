// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:28 BRT
// Last edited on June 26 of 2018, at 19:28 BRT

#include <chicago/arch/pmm.h>
#include <chicago/arch/pmm-int.h>
#include <chicago/arch/vmm.h>

#include <chicago/alloc.h>
#include <chicago/list.h>
#include <chicago/virt.h>

PList VirtRegions = Null;
PUInt32 VirtFrameMap = Null;
UInt32 VirtFrameUsed = 0;

UInt32 VirtFirstFreeFrame(UInt32 count) {
	if (count == 0) {
		return 0;
	}
	
	for (UInt32 i = 0; i < 0x6008; i++) {
		if (VirtFrameMap[i] != 0xFFFFFFFF) {
			for (UInt32 j = (i == 0) ? 1 : 0; j < 32; j++) {
				if (!(VirtFrameMap[i] & (1 << j))) {
					for (UInt32 k = 0, l = 0; k < count; k++) {
						if (!(TEST_BIT(VirtFrameMap, (i * 32) + (1 << j) + k))) {
							l++;
						} else {
							l = 0;
						}
						
						if (l == count) {
							return i * 4 * 8 + j;
						}
					}
				}
			}
		}
	}
	
	return 0;
}

UInt32 VirtAllocFrame(UInt32 count) {
	if (count == 0) {
		return 0;
	} else if ((0xC0100 - VirtFrameUsed) < count) {																										// The frame map is full?
		return 0;																																		// Yes, so we can't do anything...
	}
	
	UInt32 frame = VirtFirstFreeFrame(count);																											// Try to get the first free frame chain
	
	if (frame == 0) {																																	// Ok?
		return 0;																																		// Nope, return Falsle
	}
	
	for (UInt32 i = 0; i < count; i++) {
		SET_BIT(VirtFrameMap, frame + i);																												// Set the bit in the frame map
		VirtFrameUsed++;																																// Increase the used var
	}
	
	return frame * 4096;																																// Multiply by 4096 (page size)
}

Void VirtReserveFrame(UInt32 frame, UInt32 count) {
	if ((frame == 0) || (count == 0)) {																													// Sanity checks!
		return;
	} else if ((frame % 0x1000) != 0) {
		return;
	}
	
	for (UInt32 i = 0; i < count; i++) {
		if (!(TEST_BIT(VirtFrameMap, (frame / 4096) + i))) {																							// Need to set this bit?
			SET_BIT(VirtFrameMap, (frame / 4096) + i);																									// Yes, so set it
			VirtFrameUsed++;																															// And increase the used var
		}
	}
}

Boolean VirtUsingFrame(UInt32 frame, UInt32 count) {
	if ((frame == 0) || (count == 0)) {																													// Sanity checks!
		return True;
	} else if ((frame % 0x1000) != 0) {
		return True;
	}
	
	for (UInt32 i = 0; i < count; i++) {																												// Just do bit test
		if (!(TEST_BIT(VirtFrameMap, (frame / 4096) + i))) {
			return False;
		}
	}
	
	return True;
}

Void VirtFreeFrame(UInt32 frame, UInt32 count) {
	if ((frame == 0) || (count == 0)) {																													// Sanity checks...
		return;
	} else if ((frame % 0x1000) != 0) {
		return;
	}
	
	for (UInt32 i = 0; i < count; i++) {
		if (TEST_BIT(VirtFrameMap, (frame / 4096) + i)) {																								// Need to clean this frame
			CLEAR_BIT(VirtFrameMap, (frame / 4096) + i);																								// Yes, so unset the bit in the frame map
			VirtFrameUsed--;																															// And decrement the used var
		}
	}
}

UInt32 VirtConvertFlags(UInt32 flags) {
	UInt32 flags2 = VMM_MAP_USER;																														// Convert the VIRT_ALLOC flags to VMM_MAP flags
	
	if ((flags & VIRT_ALLOC_READ) == VIRT_ALLOC_READ) {
		flags2 |= VMM_MAP_READ;
	}
	
	if ((flags & VIRT_ALLOC_WRITE) == VIRT_ALLOC_WRITE) {
		flags2 |= VMM_MAP_WRITE;
	}
	
	if ((flags & VIRT_ALLOC_EXEC) == VIRT_ALLOC_EXEC) {
		flags2 |= VMM_MAP_EXEC;
	}
	
	return flags2;
}

Boolean VirtRegionAdd(UInt32 start, UInt32 size, UInt32 flags) {
	PVirtRegion region = (PVirtRegion)MemoryAlloc(sizeof(VirtRegion));																					// Alloc the region struct
	
	if (region == Null) {																																// Failed?
		return False;																																	// Yes...
	}
	
	region->start = start;
	region->size = size;
	region->flags = flags;
	
	if (!ListAdd(VirtRegions, region)) {																												// Add to the list
		MemoryFree((UInt32)region);																														// FAILED
		return False;
	}
	
	return True;
}

Void VirtRegionDel(PVirtRegion region) {
	if (region == Null) {																																// Sanity check
		return;
	}
	
	UInt32 i = 0;
	Boolean ok = False;
	
	ListForeach(VirtRegions, j) {																														// Search for the index
		if (j->data == region) {
			ok = True;
			break;
		} else {
			i++;
		}
	}
	
	if (ok) {
		ListRemove(VirtRegions, i);																														// Found it, so delete everything about this region
		MemoryFree((UInt32)region);
	}
}

PVirtRegion VirtQuery(UInt32 addr) {
	ListForeach(VirtRegions, i) {
		PVirtRegion region = (PVirtRegion)i->data;
		
		if ((region->start <= addr) && ((region->start + region->size) > addr)) {
			return region;
		}
	}
	
	return Null;
}

UInt32 VirtAlloc(UInt32 addr, UInt32 size, UInt32 flags) {
	if (size == 0) {																																	// Sanity checks
		return 0;
	} else if ((addr % 4096) != 0) {
		return 0;
	} else if ((size % 4096) != 0) {																													// And align UP
		size += 4096 - (size % 4096);
	}
	
	if (flags == 0) {																																	// Just a lot of sanity checks
		flags |= VIRT_ALLOC_RESERVE;
	} else if (((flags & VIRT_ALLOC_RESERVE) != VIRT_ALLOC_RESERVE) && ((flags & VIRT_ALLOC_USABLE) != VIRT_ALLOC_USABLE)) {
		flags |= (VIRT_ALLOC_RESERVE | VIRT_ALLOC_USABLE);
	} else if ((addr == 0) && ((flags & VIRT_ALLOC_USABLE) == VIRT_ALLOC_USABLE) && ((flags & VIRT_ALLOC_RESERVE) != VIRT_ALLOC_RESERVE)) {
		flags |= VIRT_ALLOC_RESERVE;
	} else if (((flags & VIRT_ALLOC_WRITE) == VIRT_ALLOC_WRITE) && ((flags & VIRT_ALLOC_READ) != VIRT_ALLOC_READ)) {
		flags |= VIRT_ALLOC_READ;
	}
	
	if ((addr == 0) && (((flags & VIRT_ALLOC_RESERVE) == VIRT_ALLOC_RESERVE) && ((flags & VIRT_ALLOC_USABLE) == VIRT_ALLOC_USABLE))) {					// Reserve the virt addr, the phys addr and map it?
		UInt32 phys = PMMAllocFrame(size / 0x1000);																										// Yes, so get an free phys addr
		
		if (phys == 0) {																																// Failed?
			return 0;																																	// Yes, so we don't have memory avaliable...
		}
		
		UInt32 virt = VirtAllocFrame(size / 0x1000);																									// Alloc the virt addr
		
		if (virt == 0) {																																// Failed?
			PMMFreeFrame(phys, size / 0x1000);																											// Yes, so we don't have more virtual address space avaliable...
			return 0;
		}
		
		UInt32 flags2 = VirtConvertFlags(flags);																										// Convert the flags
		
		for (UInt32 i = 0; i < size; i += 0x1000) {																										// And try to map everything
			if (!VMMMap(virt + i, phys + i, flags2)) {
				if (i >= 0x1000) {																														// Failed, so undo everything														
					for (UInt32 j = 0; j < i; j += 0x1000) {
						VMMUnmap(virt + i);
					}
				}
				
				PMMFreeFrame(phys, size / 0x1000);
				VirtFreeFrame(virt, size / 0x1000);
				
				return 0;
			}
		}
		
		if (!VirtRegionAdd(virt, size, flags)) {																										// Try to add it to the list
			for (UInt32 i = 0; i < size; i+= 0x1000) {																									// Failed, undo everything
				VMMUnmap(virt + i);
			}
			
			PMMFreeFrame(phys, size / 0x1000);
			VirtFreeFrame(virt, size / 0x1000);
				
			return 0;
		}
		
		return virt;
	} else if ((addr == 0) && (((flags & VIRT_ALLOC_RESERVE) == VIRT_ALLOC_RESERVE))) {																	// Just reserve?
		UInt32 virt = VirtAllocFrame(size / 0x1000);																									// Yes
		
		if (virt == 0) {
			return 0;
		}
		
		if (!VirtRegionAdd(virt, size, flags)) {
			VirtFreeFrame(virt, size / 0x1000);
			return 0;
		}
		
		return virt;
	} else if (addr >= 0xC0100000) {																													// Outside userspace?
		return 0;																																		// ...
	} else if (((flags & VIRT_ALLOC_RESERVE) == VIRT_ALLOC_RESERVE) && ((flags & VIRT_ALLOC_USABLE) == VIRT_ALLOC_USABLE)) {							// Same as the other one, but here we need to reserve the virt addr instead of allocating it
		PVirtRegion region = VirtQuery(addr);
		
		if (region != Null) {
			return 0;
		}
		
		UInt32 phys = PMMAllocFrame(size / 0x1000);
		
		if (phys == 0) {
			return 0;
		}
		
		VirtReserveFrame(addr, size / 0x1000);
		
		UInt32 flags2 = VirtConvertFlags(flags);
		
		for (UInt32 i = 0; i < size; i += 0x1000) {
			if (!VMMMap(addr + i, phys + i, flags2)) {
				if (i >= 0x1000) {
					for (UInt32 j = 0; j < i; j += 0x1000) {
						VMMUnmap(addr + i);
					}
				}
				
				PMMFreeFrame(phys, size / 0x1000);
				VirtFreeFrame(addr, size / 0x1000);
				
				return 0;
			}
		}
		
		if (!VirtRegionAdd(addr, size, flags)) {
			for (UInt32 i = 0; i < size; i+= 0x1000) {
				VMMUnmap(addr + i);
			}
			
			PMMFreeFrame(phys, size / 0x1000);
			VirtFreeFrame(addr, size / 0x1000);
				
			return 0;
		}
		
		return addr;
	} else if ((flags & VIRT_ALLOC_RESERVE) == VIRT_ALLOC_RESERVE) {																					// Again, same as the other one
		PVirtRegion region = VirtQuery(addr);
		
		if (region != Null) {
			return 0;
		}
		
		VirtReserveFrame(addr, size / 0x1000);
		
		if (!VirtRegionAdd(addr, size, flags)) {
			VirtFreeFrame(addr, size / 0x1000);
			return 0;
		}
		
		return addr;
	} else if ((flags & VIRT_ALLOC_USABLE) == VIRT_ALLOC_USABLE) {																						// AND AGAIN
		PVirtRegion region = VirtQuery(addr);
		
		if (region == Null) {
			if (VirtAlloc(addr, size, flags | VIRT_ALLOC_RESERVE) == 0) {																				// Alright, this time we can set the VIRT_ALLOC_RESERVE flag if it isn't set
				return 0;
			}
			
			region = VirtQuery(addr);
			
			if (region == Null) {
				return 0;
			}
		}
		
		UInt32 phys = PMMAllocFrame(size / 0x1000);
		
		if (phys == 0) {
			return 0;
		}
		
		UInt32 flags2 = VirtConvertFlags(flags);
		
		for (UInt32 i = 0; i < size; i += 0x1000) {
			if (!VMMMap(addr + i, phys + i, flags2)) {
				if (i >= 0x1000) {
					for (UInt32 j = 0; j < i; j += 0x1000) {
						VMMUnmap(addr + i);
					}
				}
				
				PMMFreeFrame(phys, size / 0x1000);
				
				return 0;
			}
		}
		
		region->flags = flags | VIRT_ALLOC_RESERVE;
		
		return addr;
	} else {
		return 0;
	}
}

Boolean VirtProtect(UInt32 addr, UInt32 flags) {
	if (((flags & VIRT_ALLOC_RESERVE) == VIRT_ALLOC_RESERVE) || ((flags & VIRT_ALLOC_USABLE) == VIRT_ALLOC_USABLE)) {									// Nope, you can't change those flags
		return False;
	}
	
	PVirtRegion region = VirtQuery(addr);																												// Query!
	
	if (region == Null) {
		return False;
	} else if ((region->flags & VIRT_ALLOC_USABLE) != VIRT_ALLOC_USABLE) {																				// Just reserved?
		region->flags = flags | VIRT_ALLOC_RESERVE;																										// Yes
		return True;
	}
	
	UInt32 phys = VMMGetPhys(region->start);																											// Get the phys addr
	UInt32 flags2 = VirtConvertFlags(flags);																											// Convert the flags
	
	for (UInt32 i = 0; i < region->size; i += 0x1000) {																									// And try to remap everything with the new flags (return False if it fails)
		if (!VMMMap(region->start + i, phys + i, flags2)) {
			return False;
		}
	}
	
	region->flags = flags | VIRT_ALLOC_RESERVE | VIRT_ALLOC_USABLE;	
	
	return True;
}

Void VirtFree(UInt32 addr) {	
	PVirtRegion region = VirtQuery(addr);																												// Query!
	
	if (region == Null) {
		return;
	} else if ((region->flags & VIRT_ALLOC_USABLE) != VIRT_ALLOC_USABLE) {																				// Just reserved?
		VirtFreeFrame(region->start, region->size / 0x1000);																							// Yes, so just free the virt region
		VirtRegionDel(region);																															// And delete this from the list
	
	return;
	}
	
	PMMFreeFrame(VMMGetPhys(region->start), region->size / 0x1000);																						// Free the physical memory
	
	for (UInt32 i = 0; i < region->size; i += 0x1000) {																									// Unmap everything
		VMMUnmap(region->start + i);
	}
	
	VirtFreeFrame(region->start, region->size / 0x1000);																								// Free the virt region
	VirtRegionDel(region);																																// And delete this from the list
}

Boolean VirtInit(Void) {
	VirtRegions = ListNew();
	VirtFrameMap = (PUInt32)MemoryAlloc(0x6008);
	VirtFrameUsed = 1;
	
	if ((VirtRegions == Null) || (VirtFrameMap == Null)) {
		return False;
	}
	
	return True;
}