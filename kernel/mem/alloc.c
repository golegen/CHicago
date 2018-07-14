// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:44 BRT
// Last edited on July 14 of 2018, at 00:36 BRT

#include <chicago/alloc-int.h>
#include <chicago/heap.h>

PVoid MemoryAllocBase = Null;

static PMemoryAllocBlock MemoryAllocExtendHeap(PMemoryAllocBlock last, UIntPtr size) {
	if (!size) {																	// We need a size...
		return Null;
	}
	
	PMemoryAllocBlock block = (PMemoryAllocBlock)HeapGetCurrent();					// Let's try to expand the heap!
	UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
	
	if (!HeapIncrement(size + hsize)) {
		return Null;																// Failed...
	}
	
	block->size = size;
	block->free = False;
	block->addr = (UIntPtr)block->data;
	block->next = Null;
	block->prev = last;
	
	if (last != Null) {
		last->next = block;
	}
	
	return block;
}

static PMemoryAllocBlock MemoryAllocFindBlock(PMemoryAllocBlock *last, UIntPtr size) {
	PMemoryAllocBlock block = MemoryAllocBase;
	
	while ((block != Null) && (!((block->free) && (block->size >= size)))) {		// Check if the block is free and if it's equal or greater than specified size
		*last = block;																// YES!
		block = block->next;
	}
	
	return block;
}

static Void MemoryAllocSplitBlock(PMemoryAllocBlock block, UIntPtr size) {
	PMemoryAllocBlock new = (PMemoryAllocBlock)(block->data + size);
	UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
	
	new->size = block->size - size - hsize;
	new->free = True;
	new->addr = (UIntPtr)new->data;
	new->next = block->next;
	new->prev = block;
	
	block->size = size;
	block->next = new;
	
	if (new->next != Null) {
		new->next->prev = new;
	}
}

static PMemoryAllocBlock MemoryAllocFuseBlock(PMemoryAllocBlock block) {
	UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
	
	if ((block->next != Null) && (block->next->free)) {
		block->size += hsize + block->next->size;
		block->next = block->next->next;
		
		if (block->next) {
			block->next->prev = block;
		}
	}
	
	return block;
}

static Void MemoryAllocCopyBlock(PMemoryAllocBlock src, PMemoryAllocBlock dst) {
	PUIntPtr from = (PUIntPtr)src->addr;
	PUIntPtr to = (PUIntPtr)dst->addr;
	
	for (UIntPtr i = 0; ((i * sizeof(UIntPtr)) < src->size) && ((i * sizeof(UIntPtr)) < dst->size); i++) {
		to[i] = from[i];
	}
}

UIntPtr MemoryAlloc(UIntPtr size) {
	if (!size) {
		return 0;
	}
	
	PMemoryAllocBlock block;
	PMemoryAllocBlock last;
	UIntPtr rsize = ((((size - 1) >> (sizeof(UIntPtr) - 1)) << (sizeof(UIntPtr) - 1)) + sizeof(UIntPtr));
	
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
	
	return (UIntPtr)block->data;
}

UIntPtr MemoryAlignedAlloc(UIntPtr size, UIntPtr align) {
	if (size == 0) {
		return 0;
	} else if (align == 0) {
		return 0;
	} else if ((align & (align - 1)) != 0) {
		return 0;
	}
	
	UIntPtr p1 = 0;
	PUIntPtr p2 = Null;
	Int off = align - 1 + sizeof(UIntPtr);
	
	if ((p1 = MemoryAlloc(size + off)) == 0) {
		return 0;
	}
	
	p2 = (PUIntPtr)((p1 + off) & ~(align - 1));
	p2[-1] = p1;
	
	return (UIntPtr)p2;
}

Void MemoryFree(UIntPtr blockk) {
	if (blockk == 0) {																// Some checks...
		return;
	} else if (blockk < (UIntPtr)MemoryAllocBase) {
		return;
	} else if (blockk > HeapGetCurrent()) {
		return;
	}
	
	UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
	PMemoryAllocBlock block = (PMemoryAllocBlock)(blockk - hsize);					// Let's get the block struct
	
	block->free = True;
	
	if ((block->prev != Null) && (block->prev->free)) {								// Fuse with the prev?
		block = MemoryAllocFuseBlock(block->prev);									// Yes!
	}
	
	if (block->next != Null) {														// Fuse with the next?
		MemoryAllocFuseBlock(block);												// Yes!
	} else {
		if (block->prev != Null) {													// Free the end of the heap
			block->prev->next = Null;
		} else {
			MemoryAllocBase = Null;													// No more blocks!
		}
		
		UIntPtr decr = HeapGetCurrent() - (UIntPtr)block;
		
		HeapDecrement(decr);														// Now let's decrement the heap!
	}
}

Void MemoryAlignedFree(UIntPtr block) {
	MemoryFree(((PUIntPtr)block)[-1]);
}

UIntPtr MemoryZAlloc(UIntPtr size) {
	UIntPtr ret = MemoryAlloc(size);
	
	if (ret) {
		UIntPtr rsize = ((((size - 1) >> (sizeof(UIntPtr) - 1)) << (sizeof(UIntPtr) - 1)) + sizeof(UIntPtr));
		PUIntPtr ptr = (PUIntPtr)ret;
		
		for (UIntPtr i = 0; i < rsize / sizeof(UIntPtr); i++) {
			ptr[i] = 0;
		}
	}
	
	return ret;
}

UIntPtr MemoryAlignedZAlloc(UIntPtr size, UIntPtr align) {
	UIntPtr ret = MemoryAlignedAlloc(size, align);
	
	if (ret) {
		UIntPtr rsize = ((((size - 1) >> (sizeof(UIntPtr) - 1)) << (sizeof(UIntPtr) - 1)) + sizeof(UIntPtr));
		PUIntPtr ptr = (PUIntPtr)ret;
		
		for (UIntPtr i = 0; i < rsize / sizeof(UIntPtr); i++) {
			ptr[i] = 0;
		}
	}
	
	return ret;
}

UIntPtr MemoryRealloc(UIntPtr blockk, UIntPtr size) {
	if (size == 0) {
		return 0;
	} else if (blockk == 0) {
		return MemoryAlloc(size);
	} else if (blockk < (UIntPtr)MemoryAllocBase) {
		return 0;
	} else if (blockk > HeapGetCurrent()) {
		return 0;
	} else {
		UIntPtr rsize = ((((size - 1) >> (sizeof(UIntPtr) - 1)) << (sizeof(UIntPtr) - 1)) + sizeof(UIntPtr));
		UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
		PMemoryAllocBlock block = (PMemoryAllocBlock)(blockk - hsize);
		
		if (block->size >= rsize) {
			if ((block->size - rsize) >= 24) {
				MemoryAllocSplitBlock(block, rsize);
			}
		} else {
			if ((block->next != Null) && (block->next->free) && ((block->size - hsize + block->next->size) >= rsize)) {
				MemoryAllocFuseBlock(block);
				
				if (block->size - rsize >= 24) {
					MemoryAllocSplitBlock(block, rsize);
				}
			} else {
				UIntPtr new = MemoryAlloc(size);
				
				if (new == 0) {
					return 0;
				}
				
				MemoryAllocCopyBlock(block, (PMemoryAllocBlock)(new - hsize));
				MemoryFree(blockk);
				
				return new;
			}
		}
		
		return blockk;
	}
}

UIntPtr MemoryAlignedRealloc(UIntPtr blockkk, UIntPtr size, UIntPtr align) {
	UIntPtr blockk = (UIntPtr)(((PUIntPtr)blockkk)[-1]);
	
	if (size == 0) {
		return 0;
	} else if (blockk == 0) {
		return MemoryAlloc(size);
	} else if (blockk < (UIntPtr)MemoryAllocBase) {
		return 0;
	} else if (blockk > HeapGetCurrent()) {
		return 0;
	} else {
		UIntPtr rsize = ((((size - 1) >> (sizeof(UIntPtr) - 1)) << (sizeof(UIntPtr) - 1)) + sizeof(UIntPtr));
		UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
		PMemoryAllocBlock block = (PMemoryAllocBlock)(blockk - hsize);
		
		if (block->size >= rsize) {
			if ((block->size - rsize) >= 24) {
				MemoryAllocSplitBlock(block, rsize);
			}
		} else {
			if ((block->next != Null) && (block->next->free) && ((block->size - size + block->next->size) >= rsize)) {
				MemoryAllocFuseBlock(block);
				
				if (block->size - rsize >= 24) {
					MemoryAllocSplitBlock(block, rsize);
				}
			} else {
				UIntPtr new = MemoryAlignedAlloc(size, align);
				
				if (new == 0) {
					return 0;
				}
				
				MemoryAllocCopyBlock(block, (PMemoryAllocBlock)(new - size));
				MemoryAlignedFree(blockk);
				
				return new;
			}
		}
		
		return blockk;
	}
}
