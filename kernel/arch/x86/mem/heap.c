// File author is Ítalo Lima Marconato Matias
//
// Created on June 04 of 2018, at 15:57 BRT
// Last edited on June 22 of 2018, at 19:15 BRT

#define __CHICAGO_HEAP__

#include <chicago/arch/heap-int.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/pmm-int.h>
#include <chicago/arch/vmm.h>

#include <chicago/alloc.h>
#include <chicago/heap.h>

PHeap KernelHeap = Null;
UInt32 HeapCount = 0;
PChar HeapDefaultName = "Heap0";

PHeap HeapCreate(PChar name, UInt32 start) {
	PHeap ourheap = Null;
	PHeapPrivateData ourpriv = Null;
	PChar ourname = Null;
	
	if ((start % 4096) != 0) {																		// The start is page aligned?
		return Null;																				// No...
	}
	
	if (KernelRealEnd == 0) {																		// Use PMMDumbAlloc?
		ourheap = (PHeap)PMMDumbAlloc(sizeof(Heap), False);											// Yes!
		ourpriv = (PHeapPrivateData)PMMDumbAlloc(sizeof(HeapPrivateData), False);
		
		if (name == Null) {																			// Use our default name?
			ourname = (PChar)PMMDumbAlloc(6 * sizeof(Char), False);									// Yes, so alloc space for the name
			
			for (Int i = 0; HeapDefaultName[i]; i++) {												// Copy it!
				ourname[i] = HeapDefaultName[i];
			}
			
			ourname[4] = (Char)(HeapCount + '0');
			ourname[5] = 0;
		} else {
			UInt32 ournamesize = 0;																	// No, so let's get the chosen name size
			
			for (; name[ournamesize]; ournamesize++) ;
			
			ourname = (PChar)PMMDumbAlloc((ournamesize + 1) * sizeof(Char), False);					// Alloc space
			
			for (Int i = 0; name[i]; i++) {															// And copy it!
				ourname[i] = name[i];
			}
			
			ourname[ournamesize] = 0;
		}
		
		ourheap->is_kernel = True;																	// Set the kernel bit!
	} else {
		ourheap = (PHeap)MemoryAlloc(sizeof(Heap));													// Let's try to alloc everything...
		
		if (ourheap == Null) {
			return Null;
		}
		
		ourpriv = (PHeapPrivateData)MemoryAlloc(sizeof(HeapPrivateData));
		
		if (ourpriv == Null) {
			MemoryFree((UInt32)ourheap);
			return Null;
		}
		
		if (name == Null) {																			// Use our default name?
			ourname = (PChar)MemoryAlloc(6 * sizeof(Char));											// Yes, so alloc space for the name (if we can...)
			
			if (ourname == Null) {
				MemoryFree((UInt32)ourpriv);
				MemoryFree((UInt32)ourheap);
				
				return Null;
			}
			
			for (Int i = 0; HeapDefaultName[i]; i++) {												// Copy it!
				ourname[i] = HeapDefaultName[i];
			}
			
			ourname[4] = (Char)(HeapCount + '0');
			ourname[5] = 0;
		} else {
			UInt32 ournamesize = 0;																	// No, so let's get the chosen name size
			
			for (; name[ournamesize]; ournamesize++) ;
			
			ourname = (PChar)MemoryAlloc((ournamesize + 1) * sizeof(Char));							// Alloc space
			
			if (ourname == Null) {
				MemoryFree((UInt32)ourpriv);
				MemoryFree((UInt32)ourheap);
				
				return Null;
			}
			
			for (Int i = 0; name[i]; i++) {															// And copy it!
				ourname[i] = name[i];
			}
			
			ourname[ournamesize] = 0;
		}
	}
	
	ourheap->signature = 0x0CAFEEC1;
	ourheap->name = ourname;
	ourheap->start = start;
	ourheap->current_position = start;
	ourheap->private_data = ourpriv;
	
	ourpriv->current_position_aligned = start;
	
	if (HeapCount >= 9) {
		HeapCount = 0;
	} else {
		HeapCount++;
	}
	
	return ourheap;																					// Return!
}

Void HeapDelete(PHeap heap) {
	if (heap == Null) {																				// Heap is valid?
		return;																						// Nope...
	} else if (heap->signature != 0x0CAFEEC1) {														// Heap is valid? (again)
		return;																						// Nope...
	} else if (heap->is_kernel) {																	// Kernel heap?
		return;																						// Yes... WTF???
	} else {
		PHeapPrivateData priv = (PHeapPrivateData)(heap->private_data);
		
		for (UInt32 i = heap->start; i < priv->current_position_aligned; i += 0x1000) {				// Let's free the memory we used
			PMMFreeFrame(VMMGetPhys(i));
			VMMUnmap(i);
		}
		
		MemoryFree((UInt32)heap->name);
		MemoryFree((UInt32)heap->private_data);
		MemoryFree((UInt32)heap);
	}
}

PChar HeapGetName(PHeap heap) {
	if (!heap) {																					// Heap is valid?
		return Null;																				// Nope..
	} else if (heap->signature != 0x0CAFEEC1) {														// Heap is valid? (again)
		return Null;																				// Nope...
	} else {
		return heap->name;
	}
}

UInt32 HeapGetStart(PHeap heap) {
	if (!heap) {																					// Heap is valid?
		return 0;																					// Nope..
	} else if (heap->signature != 0x0CAFEEC1) {														// Heap is valid? (again)
		return 0;																					// Nope...
	} else {
		return heap->start;
	}
}

UInt32 HeapGetCurrent(PHeap heap) {
	if (!heap) {																					// Heap is valid?
		return 0;																					// Nope..
	} else if (heap->signature != 0x0CAFEEC1) {														// Heap is valid? (again)
		return 0;																					// Nope...
	} else {
		return heap->current_position;
	}
}

Boolean HeapIncrement(PHeap heap, UInt32 amount) {
	if (!heap) {																					// Heap is valid?
		return False;																				// Nope..
	} else if (heap->signature != 0x0CAFEEC1) {														// Heap is valid? (again)
		return False;																				// Nope...
	} else if (amount <= 0) {																		// Greater than 0 amount?
		return False;																				// Nope, so it's invalid for us
	} else {
		PHeapPrivateData priv = (PHeapPrivateData)heap->private_data;
		UInt32 new = heap->current_position + amount;
		UInt32 old = priv->current_position_aligned;
		
		for (; priv->current_position_aligned < new; priv->current_position_aligned += 0x1000) {	// So let's GO!
			UInt32 phys = PMMAllocFrame();															// Alloc a new page
			
			if (phys == 0) {																		// Have space?
				priv->current_position_aligned = old;												// Nope, return with False
				return False;
			}
			
			VMMMap(priv->current_position_aligned, phys, VMM_MAP_KDEFAULTS);						// Yes, map it
		}
		
		heap->current_position += amount;
		
		return True;
	}
}

Boolean HeapDecrement(PHeap heap, UInt32 amount) {
	if (!heap) {																					// Heap is valid?
		return False;																				// Nope..
	} else if (heap->signature != 0x0CAFEEC1) {														// Heap is valid? (again)
		return False;																				// Nope...
	} else if (amount <= 0) {																		// Greater than 0 amount?
		return False;																				// Nope, so it's invalid for us
	} else if ((heap->current_position - amount) < heap->start) {									// Tried to decrement outside it's limit?
		return False;																				// Same as above
	} else {
		heap->current_position -= amount;
		return True;
	}
}
