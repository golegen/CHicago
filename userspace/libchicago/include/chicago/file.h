// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 18:01 BRT
// Last edited on November 15 of 2018, at 18:02 BRT

#ifndef __CHICAGO_FILE_H__
#define __CHICAGO_FILE_H__

#include <chicago/types.h>

#define FS_FLAG_DIR 0x01
#define FS_FLAG_FILE 0x02

#define FS_POS_START 0x00
#define FS_POS_CURRENT 0x01
#define FS_POS_END 0x02

IntPtr FsOpenFile(PChar path);
Void FsCloseFile(IntPtr file);
Boolean FsReadFile(IntPtr file, UIntPtr size, PUInt8 buf);
Boolean FsWriteFile(IntPtr file, UIntPtr size, PUInt8 buf);
Boolean FsMountFile(PChar path, PChar file, PChar type);
Boolean FsUmountFile(PChar path);
Boolean FsReadDirectoryEntry(IntPtr dir, UIntPtr entry, PChar out);
IntPtr FsFindInDirectory(IntPtr dir, PChar name);
Boolean FsCreateFile(IntPtr dir, PChar name, UIntPtr type);
Boolean FsControlFile(IntPtr file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
UIntPtr FsGetSize(IntPtr file);
UIntPtr FsGetPosition(IntPtr file);
Boolean FsSetPosition(IntPtr file, IntPtr base, UIntPtr off);

#endif		// __CHICAGO_FILE_H__

