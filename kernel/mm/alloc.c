// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:44 BRT
// Last edited on July 16 of 2018, at 19:48 BRT

#include <chicago/alloc-int.h>
#include <chicago/heap.h>
#include <chicago/string.h>

PVoid MemAllocateBase = Null;

static PMemAllocateBlock MemAllocateExtendHeap(PMemAllocateBlock last, UIntPtr size) {
	if (!size) {																	// We need a size...
		return Null;
	}
	
	PMemAllocateBlock block = (PMemAllocateBlock)HeapGetCurrent();					// Let's try to expand the heap!
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

static PMemAllocateBlock MemAllocateFindBlock(PMemAllocateBlock *last, UIntPtr size) {
	PMemAllocateBlock block = MemAllocateBase;
	
	while ((block != Null) && (!((block->free) && (block->size >= size)))) {		// Check if the block is free and if it's equal or greater than specified size
		*last = block;																// YES!
		block = block->next;
	}
	
	return block;
}

static Void MemAllocateSplitBlock(PMemAllocateBlock block, UIntPtr size) {
	PMemAllocateBlock new = (PMemAllocateBlock)(block->data + size);
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

static PMemAllocateBlock MemAllocateFuseBlock(PMemAllocateBlock block) {
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

static Void MemAllocateCopyBlock(PMemAllocateBlock src, PMemAllocateBlock dst) {
	PUIntPtr from = (PUIntPtr)src->addr;
	PUIntPtr to = (PUIntPtr)dst->addr;
	
	for (UIntPtr i = 0; ((i * sizeof(UIntPtr)) < src->size) && ((i * sizeof(UIntPtr)) < dst->size); i++) {
		to[i] = from[i];
	}
}

UIntPtr MemAllocate(UIntPtr size) {
	if (!size) {
		return 0;
	}
	
	PMemAllocateBlock block;
	PMemAllocateBlock last;
	UIntPtr rsize = ((((size - 1) >> (sizeof(UIntPtr) / 2)) << (sizeof(UIntPtr) / 2)) + sizeof(UIntPtr));
	
	if (MemAllocateBase) {															// First time using it?
		last = MemAllocateBase;														// No, so let's try to find a free block
		block = MemAllocateFindBlock(&last, rsize);
		
		if (block) {																// Found?
			if ((block->size - rsize) >= 16) {										// Yes! Let's try to split it
				MemAllocateSplitBlock(block, rsize);
			}
			
			block->free = False;													// And.. NOW THIS BLOCK BELONG TO US
		} else {
			block = MemAllocateExtendHeap(last, rsize);								// No, so let's (try to) expand the heap
			
			if (block == Null) {
				return 0;
			}
		}
	} else {
		block = MemAllocateExtendHeap(Null, rsize);									// Yes, so let's (try to) init!
		
		if (!block) {
			return 0;
		}
		
		MemAllocateBase = block;
	}
	
	return (UIntPtr)block->data;
}

UIntPtr MemAAllocate(UIntPtr size, UIntPtr align) {
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
	
	if ((p1 = MemAllocate(size + off)) == 0) {
		return 0;
	}
	
	p2 = (PUIntPtr)((p1 + off) & ~(align - 1));
	p2[-1] = p1;
	
	return (UIntPtr)p2;
}

Void MemFree(UIntPtr blockk) {
	if (blockk == 0) {																// Some checks...
		return;
	} else if (blockk < (UIntPtr)MemAllocateBase) {
		return;
	} else if (blockk >= HeapGetCurrent()) {
		return;
	}
	
	UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
	PMemAllocateBlock block = (PMemAllocateBlock)(blockk - hsize);					// Let's get the block struct
	
	block->free = True;
	
	if ((block->prev != Null) && (block->prev->free)) {								// Fuse with the prev?
		block = MemAllocateFuseBlock(block->prev);									// Yes!
	}
	
	if (block->next != Null) {														// Fuse with the next?
		MemAllocateFuseBlock(block);												// Yes!
	} else {
		if (block->prev != Null) {													// Free the end of the heap
			block->prev->next = Null;
		} else {
			MemAllocateBase = Null;													// No more blocks!
		}
		
		UIntPtr decr = HeapGetCurrent() - (UIntPtr)block;
		
		HeapDecrement(decr);														// Now let's decrement the heap!
	}
}

Void MemAFree(UIntPtr block) {
	MemFree(((PUIntPtr)block)[-1]);
}

UIntPtr MemZAllocate(UIntPtr size) {
	UIntPtr ret = MemAllocate(size);
	
	if (ret) {
		StrSetMemory((PVoid)ret, 0, size);
	}
	
	return ret;
}

UIntPtr MemReallocate(UIntPtr blockk, UIntPtr size) {
	if (size == 0) {
		return 0;
	} else if (blockk == 0) {
		return MemAllocate(size);
	} else if (blockk < (UIntPtr)MemAllocateBase) {
		return 0;
	} else if (blockk >= HeapGetCurrent()) {
		return 0;
	} else {
		UIntPtr rsize = ((((size - 1) >> (sizeof(UIntPtr) / 2)) << (sizeof(UIntPtr) / 2)) + sizeof(UIntPtr));
		UIntPtr hsize = (sizeof(UIntPtr) * 4) + sizeof(UInt32);
		PMemAllocateBlock block = (PMemAllocateBlock)(blockk - hsize);
		
		if (block->size >= rsize) {
			if ((block->size - rsize) >= 24) {
				MemAllocateSplitBlock(block, rsize);
			}
		} else {
			if ((block->next != Null) && (block->next->free) && ((block->size - hsize + block->next->size) >= rsize)) {
				MemAllocateFuseBlock(block);
				
				if (block->size - rsize >= 24) {
					MemAllocateSplitBlock(block, rsize);
				}
			} else {
				UIntPtr new = MemAllocate(size);
				
				if (new == 0) {
					return 0;
				}
				
				MemAllocateCopyBlock(block, (PMemAllocateBlock)(new - hsize));
				MemFree(blockk);
				
				return new;
			}
		}
		
		return blockk;
	}
}
