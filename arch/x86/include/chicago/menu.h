// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 14:30 BRT
// Last edited on April 19 of 2019, at 17:43 BRT

#ifndef __CHICAGO_MENU_H__
#define __CHICAGO_MENU_H__

#include <chicago/types.h>

typedef struct {
	PChar name;
	PChar device;
	PChar boot_type;
	Boolean verbose;
} MenuOption, *PMenuOption;

Void MenuLoop(Void);
Void MenuInit(Void);

#endif		// __CHICAGO_MENU_H__

