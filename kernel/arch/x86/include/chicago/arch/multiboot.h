// File author is Ítalo Lima Marconato Matias
//
// Created on May 27 of 2018, at 13:38 BRT
// Last edited on September 15 of 2018, at 13:22 BRT

#ifndef __CHICAGO_ARCH_MULTIBOOT_H__
#define __CHICAGO_ARCH_MULTIBOOT_H__

#include <chicago/types.h>

typedef struct {
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
	UInt16 vbe_mode;
	UInt16 vbe_interface_segment;
	UInt16 vbe_interface_offset;
	UInt16 vbe_interface_length;
} Packed MultibootHeader, *PMultibootHeader;

typedef struct {
	UInt16 attr;
	UInt8 wina;
	UInt8 winb;
	UInt16 gran;
	UInt16 wsize;
	UInt16 sega;
	UInt16 segb;
	UInt32 real_rct_ptr;
	UInt16 pitch;
	UInt16 width;
	UInt16 height;
	UInt8 wchar;
	UInt8 ychar;
	UInt8 planes;
	UInt8 bpp;
	UInt8 banks;
	UInt8 mem_model;
	UInt8 bank_size;
	UInt8 img_pages;
	UInt8 res0;
	UInt8 red_mask;
	UInt8 red_pos;
	UInt8 green_mask;
	UInt8 green_pos;
	UInt8 blue_mask;
	UInt8 blue_pos;
	UInt8 rsv_mask;
	UInt8 rsv_pos;
	UInt8 dc_attr;
	UInt32 phys;
	UInt32 res1;
	UInt16 res2;
} Packed MultibootVbeInfo, *PMultibootVbeInfo;

typedef struct {
	UInt32 size;
	UInt32 base_low;
	UInt32 base_high;
	UInt32 length_low;
	UInt32 length_high;
	UInt32 type;
} Packed MultibootMemoryMap, *PMultibootMemoryMap;

extern UInt32 MultibootHeaderMagic;
extern PMultibootHeader MultibootHeaderPointer;

extern Boolean CPUIDCheck(Void);

#endif		// __CHICAGO_ARCH_MULTIBOOT_H__
