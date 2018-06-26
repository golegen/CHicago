// File author is Ítalo Lima Marconato Matias
//
// Created on June 26 of 2018, at 19:27 BRT
// Last edited on June 26 of 2018, at 19:27 BRT

#ifndef __CHICAGO_VIRT_H__
#define __CHICAGO_VIRT_H__

#define VIRT_ALLOC_READ 0x01
#define VIRT_ALLOC_WRITE 0x02
#define VIRT_ALLOC_EXEC 0x04
#define VIRT_ALLOC_RESERVE 0x08
#define VIRT_ALLOC_USABLE 0x10

typedef struct {
	UInt32 start;
	UInt32 size;
	UInt32 flags;
} VirtRegion, *PVirtRegion;

PVirtRegion VirtQuery(UInt32 addr);
UInt32 VirtAlloc(UInt32 addr, UInt32 size, UInt32 flags);
Boolean VirtProtect(UInt32 addr, UInt32 flags);
Void VirtFree(UInt32 addr);
Boolean VirtInit(Void);

#endif		// __CHICAGO_VIRT_H__

