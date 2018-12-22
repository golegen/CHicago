// File author is Ítalo Lima Marconato Matias
//
// Created on October 26 of 2018, at 18:45 BRT
// Last edited on December 21 of 2018, at 23:22 BRT

#ifndef __CHICAGO_SUBARCH_INT_H__
#define __CHICAGO_SUBARCH_INT_H__

#include <chicago/types.h>

typedef struct {
	UInt32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
} Int86Registers, *PInt86Registers;

typedef struct {
	UInt32 base_low;
	UInt32 base_high;
	UInt32 length_low;
	UInt32 length_high;
	UInt32 type;
	UInt32 acpi;
} Packed SMAPEntry, *PSMAPEntry;

typedef struct {
	UInt8 sig[4];
	UInt16 ver;
	UInt32 oem;
	UInt32 cap;
	PUInt16 modes;
	UInt16 mem;
} Packed VBEInfoBlock, *PVBEInfoBlock;

typedef struct {
	UInt16 attrs;
	UInt8 wina;
	UInt8 winb;
	UInt16 gran;
	UInt16 wsize;
	UInt16 sega;
	UInt16 segb;
	UInt32 realfctptr;
	UInt16 pitch;
	UInt16 width;
	UInt16 height;
	UInt8 wchar;
	UInt8 ychar;
	UInt8 planes;
	UInt8 bpp;
	UInt8 bank;
	UInt8 memmodel;
	UInt8 banksize;
	UInt8 imgpages;
	UInt8 res0;
	UInt8 rmask;
	UInt8 rpos;
	UInt8 gmask;
	UInt8 gpos;
	UInt8 bmask;
	UInt8 bpos;
	UInt8 rsvmask;
	UInt8 rsvpos;
	UInt8 dircolattr;
	UInt32 phys;
	UInt32 res1;
	UInt16 res2;
} Packed VBEModeInfoBlock, *PVBEModeInfoBlock;

Void Int86(UInt32 num, UInt32 stack);

#endif		// __CHICAGO_SUBARCH_INT_H__
