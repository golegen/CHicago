// File author is Ítalo Lima Marconato Matias
//
// Created on June 29 of 2018, at 22:30 BRT
// Last edited on December 20 of 2018, at 12:28 BRT

#include <chicago/types.h>

extern UIntPtr LoaderEnd;

UIntPtr HeapEnd = 0x7FFFF;
UIntPtr HeapStart = (UIntPtr)&LoaderEnd;
UIntPtr HeapCurrent = (UIntPtr)&LoaderEnd;

UIntPtr HeapGetCurrent(Void) {
	return HeapCurrent;
}

UIntPtr HeapGetStart(Void) {
	return HeapStart;
}

UIntPtr HeapGetEnd(Void) {
	return HeapEnd;
}

Boolean HeapIncrement(UIntPtr amount) {
	if (amount <= 0) {																		// The increment is 0?
		return False;																		// Yes, so it's invalid
	} else if ((HeapCurrent + amount) >= HeapEnd) {											// Trying to expand beyond the limit?
		return False;																		// Yes
	}
	
	HeapCurrent += amount;																	// Increment the heap
	
	return True;
}

Boolean HeapDecrement(UIntPtr amount) {
	if (amount <= 0) {																		// The decrement is 0?
		return False;																		// Yes, so it's zero
	} else if ((HeapCurrent - amount) < HeapStart) {										// Trying to decrement too much?
		return False;																		// Yes
	}
	
	HeapCurrent -= amount;																	// Decrement the heap
	
	return True;
}
