// File author is Ítalo Lima Marconato Matias
//
// Created on June 22 of 2018, at 19:07 BRT
// Last edited on June 22 of 2018, at 19:59 BRT

#ifndef __CHICAGO_ARCH_VMM_H__
#define __CHICAGO_ARCH_VMM_H__

#include <chicago/arch/multiboot.h>

#define VMM_MAP_USER 0x01
#define VMM_MAP_KERNEL 0x02
#define VMM_MAP_READ 0x04
#define VMM_MAP_WRITE 0x08
#define VMM_MAP_EXEC 0x10
#define VMM_MAP_KDEFAULTS (VMM_MAP_KERNEL | VMM_MAP_READ | VMM_MAP_WRITE)
#define VMM_MAP_UDEFAULTS (VMM_MAP_USER | VMM_MAP_READ | VMM_MAP_WRITE)

UInt32 VMMGetPhysInt(PUInt32 dir, UInt32 virt);
UInt32 VMMGetPhys(UInt32 virt);
Boolean VMMMapInt(PUInt32 dir, UInt32 virt, UInt32 phys, UInt32 flags);
Boolean VMMMap(UInt32 virt, UInt32 phys, UInt32 flags);
Boolean VMMUnmapInt(PUInt32 dir, UInt32 virt);
Boolean VMMUnmap(UInt32 virt);
PUInt32 VMMCreateDirectory(Boolean preinit);
Void VMMFreeDirectory(PUInt32 dir);
Void VMMSwitchDirectory(PUInt32 dir);
Void VMMEnable(Void);
Void VMMDisable(Void);
Void VMMFixMultiboot(PMultibootHeader mb);
Void VMMPreInit(Void);
Void VMMInit(Void);

#ifndef __CHICAGO_VMM__
extern PUInt32 VMMKernelDirectory;
extern PUInt32 VMMCurrentDirectory;
#endif

#endif		// __CHICAGO_ARCH_VMM_H__

