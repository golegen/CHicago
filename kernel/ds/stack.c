// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 15:35 BRT
// Last edited on October 13 of 2018, at 00:00 BRT

#include <chicago/alloc.h>
#include <chicago/stack.h>
#include <chicago/mm.h>

static PVoid StackAllocMemory(UIntPtr size, Boolean user) {
	if (user) {
		return (PVoid)MmAllocUserMemory(size);
	} else {
		return (PVoid)MemAllocate(size);
	}
}

static Void StackFreeMemory(PVoid data, Boolean user) {
	if (user) {
		MmFreeUserMemory((UIntPtr)data);
	} else {
		MemFree((UIntPtr)data);
	}
}

PStack StackNew(Boolean user) {
	PStack out = StackAllocMemory(sizeof(Stack), user);									// Let's allocate the space for the stack data 
	
	if (out == Null) {																	// Failed?
		return Null;																	// Yes...
	}
	
	out->last = Null;																	// "Zero" all the fields
	out->length = 0;
	out->user = user;
	
	return out;
}

Void StackFree(PStack stack) {
	PStackNode cur;
	
	while (stack->last != Null) {														// Let's pop all the entries!
		cur = stack->last;																// Save the current one
		stack->last = cur->prev;														// Pop
		StackFreeMemory(cur, stack->user);												// And free!
	}
	
	StackFreeMemory(stack, stack->user);												// Free the stack struct
}

Boolean StackPush(PStack stack, PVoid data) {
	PStackNode node = StackAllocMemory(sizeof(StackNode), stack->user);					// Let's allocate a new stack node!
	
	if (node == Null) {																	// Failed?
		return False;																	// Yes, return False
	}
	
	node->data = data;
	
	if (stack->length == 0) {															// First entry?
		stack->last = node;																// Yes!
		node->prev = Null;
	} else {
		node->prev = stack->last;														// No
		stack->last = node;
	}
	
	stack->length++;
	
	return True;
}

PVoid StackPop(PStack stack) {
	if (stack == Null || stack->length == 0) {											// We have anything to pop?
		return Null;																	// Nope
	}
	
	PStackNode cur = stack->last;														// Save the current one
	PVoid data = cur->data;																// (the data)
	
	stack->last = cur->prev;															// Pop
	stack->length--;
	
	StackFreeMemory(cur, stack->user);													// And free!
	
	return data;
}


