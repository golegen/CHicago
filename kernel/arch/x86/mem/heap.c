// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:13 BRT
// Last edited on June 26 of 2018, at 19:13 BRT

#include <chicago/arch/pmm.h>
#include <chicago/arch/pmm-int.h>
#include <chicago/arch/vmm.h>

UInt32 HeapStart = 0;
UInt32 HeapCurrent = 0;
UInt32 HeapCurrentAligned = 0;

UInt32 HeapGetCurrent(Void) {
	return HeapCurrent;
}

UInt32 HeapGetStart(Void) {
	return HeapStart;
}

Boolean HeapIncrement(UInt32 amount) {
	if (amount <= 0) {																									// The increment is 0 (or an negative number)?
		return False;																									// Yes, so it's invalid
	} else if ((HeapCurrent + amount) >= 0xFFC00000) {																	// No more virtual address space?
		return False;																									// Yes...
	}
	
	UInt32 new = HeapCurrent + amount;
	UInt32 old = HeapCurrentAligned;
	
	for (; HeapCurrentAligned < new; HeapCurrentAligned += 0x1000) {													// Let's allocate the pages that we need
		UInt32 frame = PMMAllocFrame(1);																				// Allocate one new page
		
		if (frame == 0) {																								// Failed?
			for (UInt32 i = old; i < HeapCurrentAligned; i += 0x1000) {													// Yes, so let's undo everything we did
				PMMFreeFrame(VMMGetPhys(i), 1);
				VMMUnmap(i);
			}
			
			HeapCurrentAligned = old;
			
			return False;																								// And return False
		}
		
		if (!VMMMap(HeapCurrentAligned, frame, VMM_MAP_KDEF)) {															// Let's try to map it
			for (UInt32 i = old; i < HeapCurrentAligned; i += 0x1000) {													// Failed? So undo everything
				PMMFreeFrame(VMMGetPhys(i), 1);
				VMMUnmap(i);
			}
			
			HeapCurrentAligned = old;
			
			return False;																								// And return False
		}
	}
	
	HeapCurrent = new;
	
	return True;
}

Boolean HeapDecrement(UInt32 amount) {
	if (amount <= 0) {																									// The decrement is 0 (or an negative number)?
		return False;																									// Yes, so it's invalid
	} else if ((HeapCurrent - amount) < HeapStart) {																	// Tried to decrement outside the start?
		return False;																									// Yes...
	} else {
		HeapCurrent -= amount;																							// Decrement the unaligned pointer
		
		while ((HeapCurrentAligned - 0x1000) > HeapCurrent) {															// And the aligned one
			HeapCurrentAligned -= 0x1000;
			
			PMMFreeFrame(VMMGetPhys(HeapCurrentAligned), 1);															// Free the physical frame for use
			VMMUnmap(HeapCurrentAligned);																				// And unmap
		}
		
		return True;
	}
}

Void HeapInit(Void) {
	HeapStart = HeapCurrent = HeapCurrentAligned = KernelRealEnd;
}
