// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 18:48 BRT
// Last edited on June 28 of 2018, at 19:23 BRT

#ifndef __CHICAGO_MM_H__
#define __CHICAGO_MM_H__

#include <chicago/types.h>

#define MM_PAGE_SIZE 0x1000

#define MM_MAP_USER 0x01
#define MM_MAP_KERNEL 0x02
#define MM_MAP_READ 0x04
#define MM_MAP_WRITE 0x08
#define MM_MAP_EXEC 0x10
#define MM_MAP_KDEF (MM_MAP_KERNEL | MM_MAP_READ | MM_MAP_WRITE)
#define MM_MAP_UDEF (MM_MAP_USER | MM_MAP_READ | MM_MAP_WRITE)

#ifndef __CHICAGO_VMM__
extern PUInt32 MmKernelDirectory;
#endif

UIntPtr MmBootAlloc(UIntPtr size, Boolean align);

UIntPtr MmAllocPage(Void);
Void MmFreePage(UIntPtr addr);
UIntPtr MmGetSize(Void);
UIntPtr MmGetUsage(Void);
UIntPtr MmGetFree(Void);

UInt32 MmGetPhysInt(PUInt32 dir, UInt32 virt);
UInt32 MmQueryInt(PUInt32 dir, UInt32 virt);
Boolean MmMapInt(PUInt32 dir, UInt32 virt, UInt32 phys, UInt32 flags);
Boolean MmUnmapInt(PUInt32 dir, UInt32 virt);
UInt32 MmGetPhys(UInt32 virt);
UInt32 MmQuery(UInt32 virt);
Boolean MmMap(UInt32 virt, UInt32 phys, UInt32 flags);
Boolean MmUnmap(UInt32 virt);
Void MmSwitchDirectory(PUInt32 dir);
Void MmEnable(Void);
Void MmDisable(Void);

#endif		// __CHICAGO_MM_H__
