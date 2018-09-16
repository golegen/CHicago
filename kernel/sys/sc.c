// File author is Ítalo Lima Marconato Matias
//
// Created on September 16 of 2018, at 12:30 BRT
// Last edited on September 16 of 2018, at 13:01 BRT

#include <chicago/alloc.h>
#include <chicago/mm.h>
#include <chicago/sc.h>
#include <chicago/string.h>
#include <chicago/version.h>
#include <chicago/virt.h>

Boolean ScCheckPointer(PVoid ptr) {
#if (MM_USER_START == 0)																																	// Let's fix an compiler warning :)
	if (((UIntPtr)ptr) >= MM_USER_END) {																													// Check if the pointer is inside of the userspace!
#else
	if (((UIntPtr)ptr) < MM_USER_START || ((UIntPtr)ptr) >= MM_USER_END) {																					// Same as above
#endif
		return False;
	} else {
		return True;	
	}
}

Int ScAppendProcessFile(PFsNode file) {
	if (file == Null) {																																		// Valid file?
		return -1;																																			// No...
	}
	
	ListForeach(PsCurrentProcess->files, i) {																												// Try to find an unused entry
		PProcessFile pf = (PProcessFile)i->data;
		
		if (pf->file == Null) {																																// Found?
			pf->file = file;																																// Yes :)
			return pf->num;
		}
	}
	
	PProcessFile pf = (PProcessFile)MemAllocate(sizeof(ProcessFile));																						// Nope, alloc an new one
	
	if (pf == Null) {																																		// Failed?
		FsCloseFile(file);																																	// Yes, close the file and return -1
		return -1;
	}
	
	pf->file = file;
	pf->num = PsCurrentProcess->lastfid++;
	
	if (!ListAdd(PsCurrentProcess->files, pf)) {																											// Try to add this file to the process file list!
		MemFree((UIntPtr)pf);																																// Failed, close the file and return -1
		FsCloseFile(file);
		return -1;
	}
	
	return pf->num;
}

Boolean ScSysGetVersion(PSystemVersion ver) {
	if (!ScCheckPointer(ver)) {																																// Check if the pointer is inside of the userspace
		return False;
	}
	
	if (ver->major) {																																		// And let's do it!
		*ver->major = CHICAGO_MAJOR;
	}
	
	if (ver->minor) {
		*ver->minor = CHICAGO_MINOR;
	}
	
	if (ver->build) {
		*ver->build = CHICAGO_BUILD;
	}
	
	if (ver->codename) {
		*ver->codename = CHICAGO_CODENAME;
	}
	
	if (ver->arch) {
		*ver->arch = CHICAGO_ARCH;
	}
	
	return True;
}

UIntPtr ScVirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags) {
	return VirtAllocAddress(addr, size, flags);																												// Just redirect
}

Boolean ScVirtFreeAddress(UIntPtr addr, UIntPtr size) {
	return VirtFreeAddress(addr, size);																														// Just redirect
}

UInt32 ScVirtQueryProtection(UIntPtr addr) {
	return VirtQueryProtection(addr);																														// Just redirect
}

Boolean ScVirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags) {
	return VirtChangeProtection(addr, size, flags);																											// Just redirect
}

UIntPtr ScPsGetTID(Void) {
	return PsCurrentThread->id;																																// Return the ID of the current thread
}

UIntPtr ScPsGetPID(Void) {
	return PsCurrentProcess->id;																															// Return the ID of the current process
}

PThreadData ScPsGetThreadData(Void) {
	return PsCurrentThread->thdata;																															// Return the current thread-specific user data
}

Void ScPsSleep(UIntPtr ms) {
	PsSleep(ms);																																			// Just redirect
}

Boolean ScPsWaitThread(UIntPtr tid) {
	PThread th = PsGetThread(tid);																															// Try to get the thread using the id
	
	if (th == Null) {																																		// Failed?
		return False;																																		// Failed...
	}
	
	PsWaitThread(th);																																		// Just redirect
	
	return True;
}

Boolean ScPsWaitProcess(UIntPtr pid) {
	PProcess proc = PsGetProcess(pid);																														// Try to get the process using the id
	
	if (proc == Null) {																																		// Failed?
		return False;																																		// Failed...
	}
	
	PsWaitProcess(proc);																																	// Just redirect
	
	return True;
}

Boolean ScPsLock(PLock lock) {
	if (!ScCheckPointer(lock)) {																															// Check if the pointer is inside of the userspace
		return False;
	}
	
	PsLock(lock);																																			// Just redirect
	
	return True;
}

Boolean ScPsUnlock(PLock lock) {
	if (!ScCheckPointer(lock)) {																															// Check if the pointer is inside of the userspace
		return False;
	}
	
	PsUnlock(lock);																																			// Just redirect
	
	return True;
}

Void ScPsExitProcess(UIntPtr val) {
	PsExitProcess(val);																																		// Just redirect
}

Void ScPsExitThread(UIntPtr val) {
	PsExitThread(val);																																		// Just redirect
}

Void ScPsForceSwitch(Void) {
	PsSwitchTask(Null);																																		// Just redirect
}

Int ScFsOpenFile(PChar path) {
	if (!ScCheckPointer(path)) {																															// Check if the pointer is inside of the userspace
		return -1;
	}
	
	PFsNode file = FsOpenFile(path);																														// Try to open the file
	
	if (file == Null) {
		return -1;																																			// Failed
	}
	
	return ScAppendProcessFile(file);																														// Append the file to the process file list
}

Boolean ScFsCloseFile(Int file) {
	if (file >= PsCurrentProcess->lastfid) {																												// Check if the pointer is inside of the userspace
		return False;
	}
	
	PProcessFile pf = ListGet(PsCurrentProcess->files, file);																								// Try to get the file
	
	if (pf == Null) {
		return False;
	}
	
	FsCloseFile(pf->file);																																	// Close it
	
	pf->file = Null;																																		// "Free" this entry
	
	return True;
}

Boolean ScFsReadFile(Int file, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if ((file >= PsCurrentProcess->lastfid) || (!ScCheckPointer(buf))) {																					// Sanity checks
		return False;
	} else {
		return FsReadFile(((PProcessFile)(ListGet(PsCurrentProcess->files, file)))->file, off, len, buf);													// And redirect
	}
}

Boolean ScFsWriteFile(Int file, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if ((file >= PsCurrentProcess->lastfid) || (!ScCheckPointer(buf))) {																					// Sanity checks
		return False;
	} else {
		return FsWriteFile(((PProcessFile)(ListGet(PsCurrentProcess->files, file)))->file, off, len, buf);													// And redirect
	}
}

Boolean ScFsMountFile(PChar path, PChar file, PChar type) {
	if ((!ScCheckPointer(path)) || (!ScCheckPointer(file)) || (!ScCheckPointer(type))) {																	// Sanity checks
		return False;	
	} else {
		return FsMountFile(path, file, type);																												// And redirect
	}
}

Boolean ScFsUmountFile(PChar path) {
	if (!ScCheckPointer(path)) {																															// Sanity checks
		return False;
	} else {
		return FsUmountFile(path);																															// And redirect
	}
}

Boolean ScFsReadDirectoryEntry(Int dir, UIntPtr entry, PChar out) {
	if ((dir >= PsCurrentProcess->lastfid) || (!ScCheckPointer(out))) {																						// Sanity checks
		return False;
	}
	
	PChar name = FsReadDirectoryEntry(((PProcessFile)(ListGet(PsCurrentProcess->files, dir)))->file, entry);												// Use the internal function
	
	if (name == Null) {
		return False;																																		// Failed (probably this entry doesn't exists)
	}
	
	StrCopyMemory(out, name, StrGetLength(name) + 1);																										// Copy the out pointer to the userspace
	MemFree((UIntPtr)name);																																	// Free the out pointer
	
	return True;
}

Int ScFsFindInDirectory(Int dir, PChar name) {
	if ((dir >= PsCurrentProcess->lastfid) || (!ScCheckPointer(name))) {																					// Sanity checks
		return -1;
	}
	
	PFsNode file = FsFindInDirectory(((PProcessFile)(ListGet(PsCurrentProcess->files, dir)))->file, name);													// Use the internal function
	
	if (file == Null) {
		return -1;																																			// Failed (probably this file doesn't exists)
	}
	
	return ScAppendProcessFile(file);																														// Append the file to the process file list
}

Boolean ScFsControlFile(Int file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	if ((file >= PsCurrentProcess->lastfid) || (!ScCheckPointer(ibuf)) || (!ScCheckPointer(obuf))) {														// Sanity checks
		return False;
	} else {
		return FsControlFile(((PProcessFile)(ListGet(PsCurrentProcess->files, file)))->file, cmd, ibuf, obuf);												// And redirect
	}
}

UIntPtr ScFsGetFileSize(Int file) {
	if (file >= PsCurrentProcess->lastfid) {																												// Sanity check
		return 0;
	}
	
	PFsNode node = ((PProcessFile)(ListGet(PsCurrentProcess->files, file)))->file;																			// Try to get the file
	
	if (node == Null) {																																		// Valid?
		return 0;																																			// Nope
	} else if ((node->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {																								// File?
		return 0;																																			// We can't read the length of an directory (sorry)
	}
	
	return node->length;
}
