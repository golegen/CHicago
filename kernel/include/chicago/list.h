// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:20 BRT
// Last edited on June 26 of 2018, at 19:20 BRT

#ifndef __CHICAGO_LIST_H__
#define __CHICAGO_LIST_H__

#include <chicago/types.h>

#define ListForeach(list, i) for (PListNode i = list->head; i != Null; i = i->next)

typedef struct ListNodeStruct {
	PVoid data;
	struct ListNodeStruct *next;
	struct ListNodeStruct *prev;
} ListNode, *PListNode;

typedef struct {
	UInt32 length;
	PListNode head;
	PListNode tail;
} List, *PList;

PList ListNew(Void);
Void ListFree(PList list);
Boolean ListAdd(PList list, PVoid data);
PVoid ListRemove(PList list, UInt32 idx);
PListNode ListGet(PList list, UInt32 idx);

#endif		// __CHICAGO_LIST_H__
