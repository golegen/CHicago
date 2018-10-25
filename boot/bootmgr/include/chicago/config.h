// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 14:24 BRT
// Last edited on October 25 of 2018, at 14:24 BRT

#ifndef __CHICAGO_CONFIG_H__
#define __CHICAGO_CONFIG_H__

#include <chicago/list.h>

typedef struct {
	PChar name;
	PChar value;
	PList attrs;
} ConfField, *PConfField;

Void ConfIterate(Void (*it)(PConfField));
Void ConfInit(Void);

#endif		// __CHICAGO_CONFIG_H__
