// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 17:46 BRT
// Last edited on November 16 of 2018, at 16:27 BRT

#include <chicago/types.h>

IntPtr FsOpenFile(PChar path) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x15), "b"((UIntPtr)path));
	return ret;
}

Void FsCloseFile(IntPtr file) {
	UIntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x16), "b"(file));
}

Boolean FsReadFile(IntPtr file, UIntPtr size, PUInt8 buf) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x17), "b"(file), "c"(size), "d"((UIntPtr)buf));
	return ret;
}

Boolean FsWriteFile(IntPtr file, UIntPtr size, PUInt8 buf) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x18), "b"(file), "c"(size), "d"((UIntPtr)buf));
	return ret;
}

Boolean FsMountFile(PChar path, PChar file, PChar type) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x19), "b"((UIntPtr)path), "c"((UIntPtr)file), "d"((UIntPtr)type));
	return ret;
}

Boolean FsUmountFile(PChar path) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1A), "b"((UIntPtr)path));
	return ret;
}

Boolean FsReadDirectoryEntry(IntPtr dir, UIntPtr entry, PChar out) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1B), "b"(dir), "c"(entry), "d"((UIntPtr)out));
	return ret;
}

IntPtr FsFindInDirectory(IntPtr dir, PChar name) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1C), "b"(dir), "c"((UIntPtr)name));
	return ret;
}

Boolean FsCreateFile(IntPtr dir, PChar name, UIntPtr type) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1D), "b"(dir), "c"((UIntPtr)name), "d"(type));
	return ret;
}

Boolean FsControlFile(IntPtr file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1E), "b"(file), "c"(cmd), "d"((UIntPtr)ibuf), "S"((UIntPtr)obuf));
	return ret;
}

UIntPtr FsGetSize(IntPtr file) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1F), "b"(file));
	return ret;
}

UIntPtr FsGetPosition(IntPtr file) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x20), "b"(file));
	return ret;
}

Boolean FsSetPosition(IntPtr file, IntPtr base, UIntPtr off) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x21), "b"(file), "c"(base), "d"(off));
	return ret;
}
