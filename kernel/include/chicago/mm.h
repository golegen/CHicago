// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 18:48 BRT
// Last edited on June 29 of 2018, at 22:27 BRT

#ifndef __CHICAGO_MM_H__
#define __CHICAGO_MM_H__

#include <chicago/types.h>

#ifndef MM_PAGE_SIZE
#define MM_PAGE_SIZE 0x1000
#endif

#define MM_MAP_USER 0x01
#define MM_MAP_KERNEL 0x02
#define MM_MAP_READ 0x04
#define MM_MAP_WRITE 0x08
#define MM_MAP_EXEC 0x10
#define MM_MAP_KDEF (MM_MAP_KERNEL | MM_MAP_READ | MM_MAP_WRITE)
#define MM_MAP_UDEF (MM_MAP_USER | MM_MAP_READ | MM_MAP_WRITE)

#ifndef __CHICAGO_VMM__
extern UIntPtr MmKernelDirectory;
#endif

UIntPtr MmBootAlloc(UIntPtr size, Boolean align);

UIntPtr MmAllocPage(Void);
Void MmFreePage(UIntPtr addr);
UIntPtr MmGetSize(Void);
UIntPtr MmGetUsage(Void);
UIntPtr MmGetFree(Void);

UIntPtr MmGetPhysInt(UIntPtr dir, UIntPtr virt);
UInt32 MmQueryInt(UIntPtr dir, UIntPtr virt);
Boolean MmMapInt(UIntPtr dir, UIntPtr virt, UIntPtr phys, UIntPtr flags);
Boolean MmUnmapInt(UIntPtr dir, UIntPtr virt);
UIntPtr MmGetPhys(UIntPtr virt);
UInt32 MmQuery(UIntPtr virt);
Boolean MmMap(UIntPtr virt, UIntPtr phys, UIntPtr flags);
Boolean MmUnmap(UIntPtr virt);
Void MmSwitchDirectory(UIntPtr dir);
Void MmEnable(Void);
Void MmDisable(Void);

#endif		// __CHICAGO_MM_H__
