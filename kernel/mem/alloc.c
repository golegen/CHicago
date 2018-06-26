// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:16 BRT
// Last edited on June 26 of 2018, at 19:25 BRT

#include <chicago/alloc-int.h>
#include <chicago/heap.h>

PMemoryAllocBlock MemoryAllocBase = Null;

PMemoryAllocBlock MemoryAllocExtendHeap(PMemoryAllocBlock last, UInt32 size) {
	if (!size) {																										// We need a size...
		return Null;
	}
	
	PMemoryAllocBlock block = (PMemoryAllocBlock)HeapGetCurrent();														// Let's try to expand the heap!
	
	if (!HeapIncrement(size + 20)) {																					// Failed?
		return Null;																									// Yes...
	}
	
	block->size = size;
	block->free = False;
	block->addr = (UInt32)block->data;
	block->next = Null;
	block->prev = last;
	
	if (last != Null) {
		last->next = block;
	}
	
	return block;
}

PMemoryAllocBlock MemoryAllocFindBlock(PMemoryAllocBlock *last, UInt32 size) {
	PMemoryAllocBlock block = MemoryAllocBase;
	
	while ((block != Null) && (!((block->free) && (block->size >= size)))) {											// Check if the block is free and have at least the required size
		*last = block;																									// YES!
		block = block->next;
	}
	
	return block;
}

Void MemoryAllocSplitBlock(PMemoryAllocBlock block, UInt32 size) {
	PMemoryAllocBlock new = (PMemoryAllocBlock)(block->data + size);
	
	new->size = block->size - size - 20;
	new->free = True;
	new->addr = (UInt32)new->data;
	new->next = block->next;
	new->prev = block;
	
	block->size = size;
	block->next = new;
	
	if (new->next != Null) {
		new->next->prev = new;
	}
}

PMemoryAllocBlock MemoryAllocFuseBlock(PMemoryAllocBlock block) {
	if ((block->next != Null) && (block->next->free)) {
		block->size += 20 + block->next->size;
		block->next = block->next->next;
		
		if (block->next) {
			block->next->prev = block;
		}
	}
	
	return block;
}

Void MemoryAllocCopyBlock(PMemoryAllocBlock src, PMemoryAllocBlock dst) {
	PUInt32 from = (PUInt32)src->addr;
	PUInt32 to = (PUInt32)dst->addr;
	
	for (UInt32 i = 0; ((i * 4) < src->size) && ((i * 4) < dst->size); i++) {
		to[i] = from[i];
	}
}

UInt32 MemoryAlloc(UInt32 size) {
	if (!size) {
		return 0;
	}
	
	PMemoryAllocBlock block;
	PMemoryAllocBlock last;
	UInt32 rsize = ((((size - 1) >> 2) << 2) + 4);
	
	if (MemoryAllocBase) {															// First time using it?
		last = MemoryAllocBase;														// No, so let's try to find a free block
		block = MemoryAllocFindBlock(&last, rsize);
		
		if (block) {																// Found?
			if ((block->size - rsize) >= 16) {										// Yes! Let's try to split it
				MemoryAllocSplitBlock(block, rsize);
			}
			
			block->free = False;													// And.. NOW THIS BLOCK BELONG TO US
		} else {
			block = MemoryAllocExtendHeap(last, rsize);								// No, so let's (try to) expand the heap
			
			if (block == Null) {
				return 0;
			}
		}
	} else {
		block = MemoryAllocExtendHeap(Null, rsize);									// Yes, so let's (try to) init!
		
		if (!block) {
			return 0;
		}
		
		MemoryAllocBase = block;
	}
	
	return (UInt32)block->data;
}

UInt32 MemoryAlignedAlloc(UInt32 size, UInt32 align) {
	if (size == 0) {
		return 0;
	} else if (align == 0) {
		return 0;
	} else if ((align & (align - 1)) != 0) {
		return 0;
	}
	
	UInt32 p1 = 0;
	PUInt32 p2 = Null;
	Int off = align - 1 + sizeof(UInt32);
	
	if ((p1 = MemoryAlloc(size + off)) == 0) {
		return 0;
	}
	
	p2 = (PUInt32)((p1 + off) & ~(align - 1));
	p2[-1] = p1;
	
	return (UInt32)p2;
}

Void MemoryFree(UInt32 blockk) {
	if (blockk == 0) {																									// Some checks...
		return;
	} else if (blockk < (UInt32)MemoryAllocBase) {
		return;
	} else if (blockk > HeapGetCurrent()) {
		return;
	}
	
	PMemoryAllocBlock block = (PMemoryAllocBlock)(blockk - 20);															// Let's get the block struct
	
	block->free = True;
	
	if ((block->prev != Null) && (block->prev->free)) {																	// Fuse with the prev?
		block = MemoryAllocFuseBlock(block->prev);																		// Yes!
	}
	
	if (block->next != Null) {																							// Fuse with the next?
		MemoryAllocFuseBlock(block);																					// Yes!
	} else {
		if (block->prev != Null) {																						// Free the end of the heap
			block->prev->next = Null;
		} else {
			MemoryAllocBase = Null;																						// No more blocks!
		}
		
		UInt32 decr = HeapGetCurrent() - (UInt32)block;
		
		HeapDecrement(decr);																							// Now let's decrement the heap!
	}
}

Void MemoryAlignedFree(UInt32 block) {
	MemoryFree(((PUInt32)block)[-1]);
}

UInt32 MemoryZAlloc(UInt32 size) {
	UInt32 ret = MemoryAlloc(size);
	
	if (ret) {
		UInt32 rsize = ((((size - 1) >> 2) << 2) + 4);
		PUInt32 ptr = (PUInt32)ret;
		
		for (UInt32 i = 0; i < rsize / 4; i++) {
			ptr[i] = 0;
		}
	}
	
	return ret;
}

UInt32 MemoryRealloc(UInt32 blockk, UInt32 size) {
	if (size == 0) {
		return 0;
	} else if (blockk == 0) {
		return MemoryAlloc(size);
	} else if (blockk < (UInt32)MemoryAllocBase) {
		return 0;
	} else if (blockk > HeapGetCurrent()) {
		return 0;
	} else {
		UInt32 rsize = ((((size - 1) >> 2) << 2) + 4);
		PMemoryAllocBlock block = (PMemoryAllocBlock)(blockk - 20);
		
		if (block->size >= rsize) {
			if ((block->size - rsize) >= 24) {
				MemoryAllocSplitBlock(block, rsize);
			}
		} else {
			if ((block->next != Null) && (block->next->free) && ((block->size - 20 + block->next->size) >= rsize)) {
				MemoryAllocFuseBlock(block);
				
				if (block->size - rsize >= 24) {
					MemoryAllocSplitBlock(block, rsize);
				}
			} else {
				UInt32 new = MemoryAlloc(size);
				
				if (new == 0) {
					return 0;
				}
				
				MemoryAllocCopyBlock(block, (PMemoryAllocBlock)(new - 20));
				MemoryFree(blockk);
				
				return new;
			}
		}
		
		return blockk;
	}
}
