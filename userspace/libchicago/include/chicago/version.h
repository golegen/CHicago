// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 18:03 BRT
// Last edited on November 15 of 2018, at 18:04 BRT

#ifndef __CHICAGO_VERSION_H__
#define __CHICAGO_VERSION_H__

#include <chicago/types.h>

typedef struct {
	PUInt32 major;
	PUInt32 minor;
	PUInt32 build;
	PChar codename;
	PChar arch;
} SystemVersion, *PSystemVersion;

Void SysGetVersion(PSystemVersion ver);

#endif		// __CHICAGO_VERSION_H__
