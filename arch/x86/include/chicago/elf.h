// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 20:11 BRT
// Last edited on January 21 of 2019, at 11:53 BRT

#ifndef __CHICAGO_ELF_H__
#define __CHICAGO_ELF_H__

#include <chicago/types.h>

#ifdef ARCH_64
#define ELF_MACHINE 0x3E
#define ELF_BASE 0xFFFF800000000000

typedef struct {
	UInt8 ident[16];
	UInt16 type;
	UInt16 machine;
	UInt32 version;
	UInt64 entry;
	UInt64 ph_off;
	UInt64 sh_off;
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
	UInt32 flags;
	UInt64 offset;
	UInt64 vaddr;
	UInt64 paddr;
	UInt64 fsize;
	UInt64 msize;
	UInt64 align;
} ELFProgramHeader, *PELFProgramHeader;
#else
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
#endif

#endif		// __CHICAGO_ELF_H__
