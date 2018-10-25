// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:44 BRT
// Last edited on October 24 of 2018, at 14:02 BRT

#include <chicago/alloc-int.h>
#include <chicago/heap.h>
#include <chicago/string.h>

PAllocBlock MemAllocateBase = Null;

static Void MemAllocateSplitBlock(PAllocBlock block, UIntPtr size) {
	if ((block == Null) || (block->size <= (size + sizeof(AllocBlock)))) {
		return;
	}
	
	PAllocBlock new = (PAllocBlock)(block->start + size);
	
	new->size = block->size - (size + sizeof(AllocBlock));
	new->start = block->start + size + sizeof(AllocBlock);
	new->free = True;
	new->next = block->next;
	new->prev = block;
	
	block->size = size;
	block->next = new;
	
	if (new->next != Null) {
		new->next->prev = new;
	}
}

static PAllocBlock MemAllocateFuseBlock(PAllocBlock block) {
	if ((block->next != Null) && (((UIntPtr)block->next) == (block->start + block->size)) && (block->next->free)) {
		block->size += sizeof(AllocBlock) + block->next->size;
		block->next = block->next->next;
		
		if (block->next != Null) {
			block->next->prev = block;
		}
	}
	
	return block;
}

static PAllocBlock MemAllocateFindBlock(PAllocBlock *last, UIntPtr size) {
	PAllocBlock block = MemAllocateBase;
	
	while ((block != Null) && (!((block->free) && (block->size >= size)))) {													// Check if the block is free and if it's equal or greater than specified size
		*last = block;																											// YES!
		block = block->next;
	}
	
	return block;
}

static PAllocBlock MemAllocateCreateBlock(PAllocBlock last, UIntPtr size) {
	if (!size) {																												// We need a size...
		return Null;
	}
	
	PAllocBlock block = (PAllocBlock)HeapGetCurrent();																			// Let's try to expand the heap!
	
	if (!HeapIncrement(size + sizeof(AllocBlock))) {
		return Null;																											// Failed...
	}
	
	block->size = size;
	block->start = ((UIntPtr)block) + sizeof(AllocBlock);
	block->free = False;
	block->next = Null;
	block->prev = last;
	
	if (block->size > (size + sizeof(AllocBlock))) {																			// Split this block if we can
		MemAllocateSplitBlock(block, size);
	}
	
	if (last != Null) {
		last->next = block;
	}
	
	return block;
}

UIntPtr MemAllocate(UIntPtr size) {
	if (size == 0) {
		return 0;
	}
	
	PAllocBlock block = Null;
	
	if ((size % sizeof(UIntPtr)) != 0) {																						// Align size to UIntPtr
		size += sizeof(UIntPtr) - (size % sizeof(UIntPtr));
	}
	
	if (MemAllocateBase != Null) {
		PAllocBlock last = MemAllocateBase;
		
		block = MemAllocateFindBlock(&last, size);
		
		if (block != Null) {																									// Found?
			if (block->size > (size + sizeof(AllocBlock))) {																	// Yes! Let's try to split it (if we can)
				MemAllocateSplitBlock(block, size);
			}
			
			block->free = False;																								// And... NOW THIS BLOCK BELONG TO US
		} else {
			block = MemAllocateCreateBlock(last, size);																			// No, so let's (try to) create a new block
			
			if (block == Null) {
				return 0;																										// Failed...
			}
		}
	} else {
		block = MemAllocateBase = MemAllocateCreateBlock(Null, size);															// Yes, so let's (try to) init
		
		if (block == Null) {
			return 0;
		}
	}
	
	return block->start;
}

Void MemFree(UIntPtr block) {
	if (block == 0) {																											// Some checks...
		return;
	} else if (block < (UIntPtr)MemAllocateBase) {
		return;
	} else if (block >= HeapGetCurrent()) {
		return;
	}
	
	PAllocBlock blk = (PAllocBlock)(block - sizeof(AllocBlock));																// Let's get the block struct
	
	blk->free = True;
	
	if ((blk->prev != Null) && (blk->prev->free)) {																				// Fuse with the prev?
		blk = MemAllocateFuseBlock(blk->prev);																					// Yes!
	}
	
	if (blk->next != Null) {																									// Fuse with the next?
		MemAllocateFuseBlock(blk);																								// Yes!
	} else {
		if (blk->prev != Null) {																								// No, so let's free the end of the heap
			blk->prev->next = Null;
		} else {
			MemAllocateBase = Null;																								// No more blocks!
		}
		
		HeapDecrement(blk->size + sizeof(AllocBlock));																			// Now let's decrement the heap!
	}
}

UIntPtr MemZAllocate(UIntPtr size) {
	UIntPtr ret = MemAllocate(size);																							// Allocate the memory
	
	if (ret) {
		StrSetMemory((PVoid)ret, 0, size);																						// And zero it
	}
	
	return ret;
}

UIntPtr MemReallocate(UIntPtr block, UIntPtr size) {
	if (size == 0) {																											// Some checks
		return 0;
	} else if (block == 0) {
		return MemAllocate(size);																								// Just allocate
	} else if (block < (UIntPtr)MemAllocateBase) {
		return 0;
	} else if (block >= HeapGetCurrent()) {
		return 0;
	}
	
	PAllocBlock blk = (PAllocBlock)(block - sizeof(AllocBlock));																// Let's get the block struct
	UIntPtr new = MemAllocate(size);																							// Alloc
	
	if (new == 0) {
		return 0;																												// Failed...
	}
	
	StrCopyMemory((PUInt8)new, (PUInt8)block, size > blk->size ? blk->size : size);												// Copy the old data
	MemFree(block);																												// Free the old memory
	
	return new;
}
