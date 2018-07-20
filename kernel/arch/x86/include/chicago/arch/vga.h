// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 15:21 BRT
// Last edited on July 20 of 2018, at 17:30 BRT

#ifndef __CHICAGO_ARCH_VGA_H__
#define __CHICAGO_ARCH_VGA_H__

#include <chicago/types.h>

#define VGA_AC_INDEX 0x3C0
#define VGA_AC_WRITE 0x3C0
#define VGA_AC_READ 0x3C1
#define VGA_MISC_WRITE 0x3C2
#define VGA_SEQ_INDEX 0x3C4
#define VGA_SEQ_DATA 0x3C5
#define VGA_DAC_READ_INDEX 0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA 0x3C9
#define VGA_MISC_READ 0x3CC
#define VGA_GC_INDEX 0x3CE
#define VGA_GC_DATA 0x3CF
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA 0x3D5
#define VGA_INSTAT_READ 0x3DA
#define VGA_NUM_SEQ_REGS 5
#define VGA_NUM_CRTC_REGS 25
#define VGA_NUM_GC_REGS 9
#define VGA_NUM_AC_REGS 21
#define VGA_NUM_REGS (1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)
#define VGA_FRAMEBUFFER 0xA0000

Void VGAPreInit(Void);
Void VGAInit(Void);

#endif		// __CHICAGO_ARCH_VGA_H__
