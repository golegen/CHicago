// File author is Ítalo Lima Marconato Matias
//
// Created on July 12 of 2018, at 21:24 BRT
// Last edited on July 12 of 2018, at 21:44 BRT

#ifndef __CHICAGO_ARCH_MULTIBOOT_H__
#define __CHICAGO_ARCH_MULTIBOOT_H__

#include <chicago/types.h>

typedef struct MultibootHeaderStruct32
{
	UInt32 flags;
	UInt32 mem_lower;
	UInt32 mem_upper;
	UInt32 boot_device;
	UInt32 cmd_line;
	UInt32 mods_count;
	UInt32 mods_address;
	UInt32 num;
	UInt32 size;
	UInt32 address;
	UInt32 shndx;
	UInt32 mmap_length;
	UInt32 mmap_address;
	UInt32 drives_length;
	UInt32 drives_address;
	UInt32 config_table;
	UInt32 boot_loader_name;
	UInt32 apm_table;
	UInt32 vbe_control_info;
	UInt32 vbe_mode_info;
	UInt32 vbe_mode;
	UInt32 vbe_interface_segment;
	UInt32 vbe_interface_offset;
	UInt32 vbe_interface_length;
} Packed MultibootHeader32, *PMultibootHeader32;

typedef struct MultibootHeaderStruct
{
	UInt64 flags;
	UInt64 mem_lower;
	UInt64 mem_upper;
	UInt64 boot_device;
	UInt64 cmd_line;
	UInt64 mods_count;
	UInt64 mods_address;
	UInt64 num;
	UInt64 size;
	UInt64 address;
	UInt64 shndx;
	UInt64 mmap_length;
	UInt64 mmap_address;
	UInt64 drives_length;
	UInt64 drives_address;
	UInt64 config_table;
	UInt64 boot_loader_name;
	UInt64 apm_table;
	UInt64 vbe_control_info;
	UInt64 vbe_mode_info;
	UInt64 vbe_mode;
	UInt64 vbe_interface_segment;
	UInt64 vbe_interface_offset;
	UInt64 vbe_interface_length;
} Packed MultibootHeader64, *PMultibootHeader64;

typedef struct MultibootMemoryMapStruct
{
	UInt32 size;
	UInt64 base;
	UInt64 length;
	UInt32 type;
} Packed MultibootMemoryMap, *PMultibootMemoryMap;

extern UInt64 MultibootHeaderMagic;
extern PMultibootHeader64 MultibootHeaderPointer;
extern PMultibootHeader32 MultibootHeaderPointer32;

#ifndef __CHICAGO_ARCH__
extern MultibootHeader64 MultibootHeaderPointer64;
#endif

#endif		// __CHICAGO_ARCH_MULTIBOOT_H__
