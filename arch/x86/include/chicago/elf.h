// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 20:11 BRT
// Last edited on December 22 of 2018, at 00:55 BRT

#ifndef __CHICAGO_ELF_H__
#define __CHICAGO_ELF_H__

#include <chicago/types.h>

#define ELF_MACHINE 0x03
#define ELF_BASE 0xC0000000

typedef struct {
	UInt8 ident[16];
	UInt16 type;
	UInt16 machine;
	UInt32 version;
	UInt32 entry;
	UInt32 ph_off;
	UInt32 sh_off;
	UInt32 flags;
	UInt16 eh_size;
	UInt16 ph_ent_size;
	UInt16 ph_num;
	UInt16 sh_ent_size;
	UInt16 sh_num;
	UInt16 sh_str_ndx;
} ELFHeader, *PELFHeader;

typedef struct {
	UInt32 type;
	UInt32 offset;
	UInt32 vaddr;
	UInt32 paddr;
	UInt32 fsize;
	UInt32 msize;
	UInt32 flags;
	UInt32 align;
} ELFProgramHeader, *PELFProgramHeader;

#endif		// __CHICAGO_ELF_H__
