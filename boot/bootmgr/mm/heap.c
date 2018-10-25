// File author is Ítalo Lima Marconato Matias
//
// Created on June 29 of 2018, at 22:30 BRT
// Last edited on October 25 of 2018, at 13:54 BRT

#include <chicago/types.h>

UIntPtr HeapEnd = 0;
UIntPtr HeapStart = 0;
UIntPtr HeapCurrent = 0;

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

Void HeapInit(UIntPtr start, UIntPtr end) {
	HeapStart = HeapCurrent = start;														// Set the start var, the curr var
	HeapEnd = end;																			// And the end var
}
