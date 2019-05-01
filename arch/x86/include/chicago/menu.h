// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 14:30 BRT
// Last edited on May 01 of 2019, at 11:21 BRT

#ifndef __CHICAGO_MENU_H__
#define __CHICAGO_MENU_H__

#include <chicago/types.h>

typedef struct {
	PChar name;
	PChar device;
	PChar boot_type;
} MenuOption, *PMenuOption;

Void MenuLoop(Void);
Void MenuInit(Void);

#endif		// __CHICAGO_MENU_H__

