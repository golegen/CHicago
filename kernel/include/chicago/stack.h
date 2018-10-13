// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 15:35 BRT
// Last edited on October 12 of 2018, at 15:35 BRT

#ifndef __CHICAGO_STACK_H__
#define __CHICAGO_STACK_H__

#include <chicago/types.h>

typedef struct StackNodeStruct {
	PVoid data;
	struct StackNodeStruct *prev;
} StackNode, *PStackNode;

typedef struct {
	PStackNode last;
	UIntPtr length;
	Boolean user;
} Stack, *PStack;

PStack StackNew(Boolean user);
Void StackFree(PStack stack);
Boolean StackPush(PStack stack, PVoid data);
PVoid StackPop(PStack stack);

#endif		// __CHICAGO_STACK_H__
