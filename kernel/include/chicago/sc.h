// File author is Ítalo Lima Marconato Matias
//
// Created on September 16 of 2018, at 12:27 BRT
// Last edited on September 16 of 2018, at 12:37 BRT

#ifndef __CHICAGO_ARCH_SC_H__
#define __CHICAGO_ARCH_SC_H__

#include <chicago/process.h>

typedef struct {
	PUInt32 major;
	PUInt32 minor;
	PUInt32 build;
	PChar *codename;
	PChar *arch;
} SystemVersion, *PSystemVersion;

Boolean ScCheckPointer(PVoid ptr);
Int ScAppendProcessFile(PFsNode file);
Boolean ScSysGetVersion(PSystemVersion ver);
UIntPtr ScVirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags);
Boolean ScVirtFreeAddress(UIntPtr addr, UIntPtr size);
UInt32 ScVirtQueryProtection(UIntPtr addr);
Boolean ScVirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags);
UIntPtr ScPsGetTID(Void);
UIntPtr ScPsGetPID(Void);
PThreadData ScPsGetThreadData(Void);
Void ScPsSleep(UIntPtr ms);
Boolean ScPsWaitThread(UIntPtr tid);
Boolean ScPsWaitProcess(UIntPtr pid);
Boolean ScPsLock(PLock lock);
Boolean ScPsUnlock(PLock lock);
Void ScPsExitProcess(UIntPtr val);
Void ScPsExitThread(UIntPtr val);
Void ScPsForceSwitch(Void);
Int ScFsOpenFile(PChar path);
Boolean ScFsCloseFile(Int file);
Boolean ScFsReadFile(Int file, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean ScFsWriteFile(Int file, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean ScFsMountFile(PChar path, PChar file, PChar type);
Boolean ScFsUmountFile(PChar path);
Boolean ScFsReadDirectoryEntry(Int dir, UIntPtr entry, PChar out);
Int ScFsFindInDirectory(Int dir, PChar name);
Boolean ScFsControlFile(Int file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
UIntPtr ScFsGetFileSize(Int file);

#endif		// __CHICAGO_ARCH_SC_H__
