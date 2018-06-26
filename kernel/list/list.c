// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:22 BRT
// Last edited on June 26 of 2018, at 19:22 BRT

#include <chicago/alloc.h>
#include <chicago/list.h>

PList ListNew(Void) {
	PList list = (PList)MemoryAlloc(sizeof(List));																		// Try to alloc a new list struct
	
	if (list == Null) {																									// Failed?
		return Null;																									// Yes...
	}
	
	list->length = 0;
	list->head = Null;
	list->tail = Null;
	
	return list;
}

Void ListFree(PList list) {
	PListNode cur;
	
	while (list->head != Null) {																						// Free all the nodes
		cur = list->head;
		list->head = cur->next;
		
		MemoryFree((UInt32)cur->data);
		MemoryFree((UInt32)cur);
	}
	
	MemoryFree((UInt32)list);																							// And the list struct
}

Boolean ListAdd(PList list, PVoid data) {
	PListNode node = (PListNode)MemoryAlloc(sizeof(ListNode));
	
	if (node == Null) {
		return False;
	}
	
	node->data = data;
	node->next = Null;
	
	if (list->length == 0) {
		list->head = node;
		node->prev = Null;
	} else {
		list->tail->next = node;
		node->prev = list->tail;
	}
	
	list->tail = node;
	list->length++;
	
	return True;
}

PVoid ListRemove(PList list, UInt32 idx) {
	if (idx > list->length) {
		return Null;
	}
	
	PListNode node = list->head;
	PVoid data = Null;
	
	for (UInt32 i = 0; i < idx; i++) {
		node = node->next;
	}
	
	data = node->data;
	
	if (node == list->head) {
		list->head = node->next;
	}
	
	if (node == list->tail) {
		list->tail = node->prev;
	}
	
	if (node->prev != Null) {
		node->prev->next = node->next;
	}
	
	if (node->next != Null) {
		node->next->prev = node->prev;
	}
	
	node->prev = Null;
	node->next = Null;
	
	MemoryFree((UInt32)node);
	
	list->length--;
	
	return data;
}

PListNode ListGet(PList list, UInt32 idx) {
	if (idx > list->length) {
		return Null;
	}
	
	PListNode node = list->head;
	
	for (UInt32 i = 0; i < idx; i++) {
		node = node->next;
	}
	
	return node;
}
