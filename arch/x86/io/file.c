// File author is Ítalo Lima Marconato Matias
//
// Created on July 16 of 2018, at 18:28 BRT
// Last edited on February 24 of 2019, at 16:44 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/file.h>
#include <chicago/string.h>

PList FsMountPointList = Null;
PList FsTypeList = Null;

PList FsTokenizePath(PChar path) {
	if (path == Null) {																													// Path is Null?
		return Null;																													// Yes...
	} else if ((StrGetLength(path) == 0) || ((StrGetLength(path) == 1) && (path[0] == '\\'))) {											// Root directory?
		return ListNew(True);																											// Yes, so just return an empty list
	}
	
	PChar clone = StrDuplicate(path);
	PList list = ListNew(True);																											// Create the tok list
	
	if (list == Null || clone == Null) {																								// Failed to alloc it?
		return Null;																													// Yes, so we can't do anything :(
	}
	
	PChar tok = StrTokenize(clone, "\\");																								// Let's tokenize it!
	
	while (tok != Null) {
		if ((StrGetLength(tok) == 2) && (StrCompare(tok, ".."))) {																		// Parent directory (..)?
			if (list->length > 0) {																										// We're in the root directory?
				MemFree((UIntPtr)(ListRemove(list, list->length - 1)));																	// No, so remove the last entry (current one)
			}
		} else if (!((StrGetLength(tok) == 1) && (StrCompare(tok, ".")))) {																// Current directory (.)?
			ListAdd(list, StrDuplicate(tok));																							// No, so add it to the list
		}
		
		tok = StrTokenize(Null, "\\");
	}
	
	MemFree((UIntPtr)clone);
	
	return list;
}

PChar FsCanonicalizePath(PChar path) {
	if (path == Null) {																													// Path is Null?
		return Null;																													// YES
	} else if (path[0] != '\\') {																										// Absolute path?
		return Null;																													// No, but we don't support relative paths in this function :(
	}
	
	PList list = ListNew(True);																											// Create the tok list
	
	if (list == Null) {																													// Failed to alloc space for it?
		return Null;																													// :(
	}
	
	PChar tok = StrTokenize(path, "\\");																								// First, let's tokenize it (if you want, take a look in the FsTokenizePath function)
	PChar final = Null;
	PChar foff = Null;
	UIntPtr fsize = 0;
	
	while (tok != Null) {
		if ((StrGetLength(tok) == 2) && (StrCompare(tok, ".."))) {
			if (list->length > 0) {
				MemFree((UIntPtr)(ListRemove(list, list->length - 1)));
			}
		} else if (!((StrGetLength(tok) == 1) && (StrCompare(tok, ".")))) {
			ListAdd(list, StrDuplicate(tok));
		}
		
		tok = StrTokenize(Null, "\\");
	}
	
	if (list->length == 0) {																											// Root directory?
		final = StrDuplicate("\\");																										// Yes
	} else {
		ListForeach(list, i) {																											// No, so let's get the final size
			fsize += StrGetLength((PChar)(i->data)) + 2;
		}
		
		final = foff = (PChar)MemAllocate(fsize);																						// Alloc space
		
		if (final == Null) {																											// Failed?
			ListFree(list);																												// Yes, so free everything and return
			return Null;
		}
		
		ListForeach(list, i) {																											// Now let's copy everything to the ret string!
			StrCopy(foff++, "\\");
			StrCopy(foff, (PChar)(i->data));
			foff += StrGetLength((PChar)(i->data));
		}
	}
	
	ListFree(list);																														// Free our list
	
	return final;																														// And return!
}

PChar FsJoinPath(PChar src, PChar incr) {
	if ((src == Null) && (incr == Null)) {																								// We need at least one of them!
		return Null;
	} else if ((src == Null) && (incr != Null)) {																						// We only have src?
		return FsCanonicalizePath(src);																									// Yes, so we can only canonicalize it
	} else if ((src != Null) && (incr == Null)) {																						// Only incr?
		return FsCanonicalizePath(incr);																								// Yes, so we can only canonicalize it
	} else if (src[0] != '\\') {																										// Absolute path?
		return Null;																													// No
	}
	
	UIntPtr psize = StrGetLength(src) + StrGetLength(incr) + 2;																			// Let's calculate the final string size
	PChar path = Null;
	PChar poff = Null;
	
	if ((incr[0] != '\\') && (src[StrGetLength(src)] != '\\')) {
		psize++;
	}
	
	path = poff = (PChar)MemAllocate(psize);																							// Allocate some space
	
	if (path == Null) {																													// Failed?
		ConWriteFormated("5\r\n");
		return Null;																													// Yes
	}
	
	StrCopy(poff, src);																													// Copy the src
	poff += StrGetLength(src);
	
	if ((incr[0] != '\\') && (src[StrGetLength(src)] != '\\')) {
		*poff++ = '\\';
	}
	
	StrCopy(poff, incr);																												// Copy the incr
	
	PChar final = FsCanonicalizePath(path);																								// Canonicalize
	
	MemFree((UIntPtr)path);																												// Free our temp path
	
	return final;																														// And return
}

Boolean FsReadFile(PFsNode file, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (file == Null) {																													// File is Null pointer?
		return False;																													// Yes, so we can't do anything...
	} else if ((file->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {																			// We're trying to read an file?
		return False;																													// ... Why are you trying to read raw bytes from an directory?
	} else if (file->read != Null) {																									// Implementation?
		return file->read(file, off, len, buf);																							// Yes, so call it!
	} else {
		return False;
	}
}

Boolean FsOpenFileInt(PFsNode node) {
	if (node == Null) {																													// Node is Null pointer?
		return False;																													// Yes... (Really, those comments are starting to get annoying)
	} else if (node->open != Null) {																									// Any implementation?
		return node->open(node);																										// Yes, so call it
	} else {
		return False;
	}
}

PFsNode FsOpenFile(PChar path) {
	if ((FsMountPointList == Null) || (path == Null)) {																					// Some null pointer checks
		return Null;
	} else if (FsMountPointList->length == 0) {																							// Don't even lose time trying to open some file if our mount point list is empty
		return Null;
	} else if (path[0] != '\\') {																										// Finally, we only support absolute paths in this function
		return Null;
	}
	
	PChar rpath = Null;
	PFsMountPoint mp = FsGetMountPoint(path, &rpath);																					// Find the mount point
	
	if ((mp == Null) || (rpath == Null)) {																								// Failed?
		return Null;																													// Yes
	}
	
	PList parts = FsTokenizePath(rpath);																								// Let's try to tokenize our path (relative to the mount point)!
	PFsNode cur = mp->root;
	
	if (parts == Null) {																												// Failed?
		return Null;																													// Yes
	}
	
	if (!FsOpenFileInt(cur)) {																											// Let's try to "open" the mount point root directory
		ListFree(parts);																												// Failed
		return Null;
	}
	
	ListForeach(parts, i) {
		cur = FsFindInDirectory(cur, (PChar)(i->data));																					// Let's try to find the file/folder in the folder (the file is i->data and the folder is cur)
		
		if (cur == Null) {
			ListFree(parts);																											// Failed
			return Null;
		}
		
		if (!FsOpenFileInt(cur)) {																										// Let's try to "open" the file/folder
			ListFree(parts);																											// Failed
			return Null;
		}
	}
	
	ListFree(parts);																													// Finally, free the list
	
	return cur;																															// And return!
}

Void FsCloseFile(PFsNode node) {
	if (node == Null) {																													// Look, except for FsOpenFile, FsMountFile and FsUmountFile, all the other function follows the same model, so if you want, just take a look at any of them!
		return;
	} else if (node->close != Null) {
		node->close(node);
	}
}

Boolean FsMountFile(PChar path, PChar file, PChar type) {
	if ((FsMountPointList == Null) || (FsTypeList == Null)) {																			// Like in all the functions, first do some null pointer checks
		return False;
	} else if ((path == Null) || (file == Null)) {																						// Same as above
		return False;
	}
	
	PFsNode src = FsOpenFile(file);																										// Try to open the source file
	
	if (src == Null) {																													// Failed (the file doesn't exists)?
		return False;																													// Yes
	} else if ((src->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {																			// The file isn't an file?
		FsCloseFile(src);																												// Yeah, but we can't mount directories...
		return False;
	}
	
	PFsType typ = FsGetType(type);																										// Let's try to get the type
	
	if ((type != Null) && (typ == Null)) {																								// If it returned Null and was an user requested type, IT FAILED, RETURN NULL
		FsCloseFile(src);
		return False;
	} else if (typ != Null) {																											// Else (if was user requested type and was found), let's do some probe
		if (typ->probe != Null) {
			if (!typ->probe(src)) {
				FsCloseFile(src);																										// Failed... we can't mount it
				return False;
			}
		}
	} else if (type == Null) {
		ListForeach(FsTypeList, i) {																									// Let's probe all the entries of the fs type list!
			PFsType tp = (PFsType)(i->data);
			
			if (tp->probe != Null) {
				if (tp->probe(src)) {
					typ = tp;																											// Found it!
					break;
				}
			}
		}
		
		if (typ == Null) {
			FsCloseFile(src);																											// We haven't found it...
			return False;
		}
	}
	
	if (typ->mount == Null) {
		FsCloseFile(src);																												// WTF? This FS doesn't have an mount function lol
		return False;
	}
	
	PFsMountPoint mp = typ->mount(src, path);																							// Try to mount it
	
	if (mp == Null) {																													// The mount failed?
		FsCloseFile(src);																												// Yes :(
		return False;
	}
	
	if (!FsAddMountPoint(mp->path, mp->type, mp->root)) {																				// And let's try to add this mount point
		MemFree((UIntPtr)mp);																											// Failed, so return False
		return False;
	} else {
		MemFree((UIntPtr)mp);																											// Otherwise, return True
		return True;
	}
}

Boolean FsUmountFile(PChar path) {
	if ((FsMountPointList == Null) || (path == Null)) {																					// Again, null pointer checks
		return False;
	}
	
	PChar rpath = Null;
	PFsMountPoint mp = FsGetMountPoint(path, &rpath);																					// Let's get the mount point info/struct
	
	if (mp == Null) {																													// Failed...
		return False;
	} else if (!StrCompare(rpath, "")) {
		return False;																													// You need to pass the absolute path to the mount point to this function!
	}
	
	PFsType type = FsGetType(mp->type);																									// Try to get the fs type info
	
	if (type != Null) {																													// Failed?
		if (type->umount != Null) {																										// No, we can call the umount function?
			if (!type->umount(mp)) {																									// Yes!
				return False;																											// Failed, return False
			}
		}
	}
	
	return FsRemoveMountPoint(path);																									// The FsRemoveMountPoint will do all the job now!
}

PChar FsReadDirectoryEntry(PFsNode dir, UIntPtr entry) {
	if (dir == Null) {
		return Null;
	} else if ((dir->flags & FS_FLAG_DIR) != FS_FLAG_DIR) {																				// Directory-only function!
		return Null;
	} else if (dir->readdir != Null) {
		return dir->readdir(dir, entry);
	} else {
		return Null;
	}
}

PFsNode FsFindInDirectory(PFsNode dir, PChar name) {
	if ((dir == Null) || (name == Null)) {
		return Null;
	} else if ((dir->flags & FS_FLAG_DIR) != FS_FLAG_DIR) {																				// Directory-only function!
		return Null;
	} else if (dir->finddir != Null) {
		return dir->finddir(dir, name);
	} else {
		return Null;
	}
}

PFsMountPoint FsGetMountPoint(PChar path, PChar *outp) {
	if ((FsMountPointList == Null) || (path == Null)) {																				// The mount point list is initialized? Path is Null?
		if (outp != Null) {																												// Nope, so let's set the outp (out pointer) to Null and return Null
			*outp = Null;
		}
		
		return Null;
	}
	
	PChar dup = StrDuplicate(path);																										// Let's duplicate the path, as we're going to change the string
	UIntPtr ncurr = StrGetLength(dup) - 1;
	
	if (!StrCompare(dup, "\\")) {																										// We're trying to find the root mount point?
		while (dup[ncurr] == '\\') {																									// No
			dup[ncurr--] = '\0';
		}
	}
	
	while (dup[0] != '\0') {
		Boolean root = StrCompare(dup, "\\");
		
		ListForeach(FsMountPointList, i) {
			PFsMountPoint mp = (PFsMountPoint)(i->data);
			
			if (StrGetLength(mp->path) == (ncurr + 1)) {																				// The length of this mount point is the same of the current length of our duplicate pointer?
				if (StrCompare(mp->path, dup)) {																						// Yes, so let's compare!
					MemFree((UIntPtr)dup);																								// WE FOUND IT! So free our duplicate, we don't need it anymore :)
					
					if (outp != Null) {																									// If the user requested, let's save the relative path
						if ((mp->path[StrGetLength(mp->path) - 1] == '\\') || (StrCompare(mp->path, path))) {							// The mount point path finishes with an slash (or we're trying to "get" the root directory of the mount point)?
							*outp = path + StrGetLength(mp->path);																		// Yes, so we can use mp->path length
						} else {
							*outp = path + StrGetLength(mp->path) + 1;																	// No, so we need to use mp->path length + 1
						}
					}
					
					return mp;																											// And return the mount point info
				}
			}
		}
		
		while (dup[ncurr] != '\\') {
			dup[ncurr--] = '\0';
		}
		
		if (!(StrCompare(dup, "\\") && !root)) {
			dup[ncurr--] = '\0';
		}
	}
	
	MemFree((UIntPtr)dup);																												// Alright, we failed (this mount point doesn't exists...)
	
	if (outp != Null) {																													// So let's set the outp (out pointer) to Null
		*outp = Null;
	}
	
	return Null;																														// And return Null
}

PFsType FsGetType(PChar type) {
	if ((FsTypeList == Null) || (type == Null)) {																						// Null pointer checks
		return Null;
	} else if (FsTypeList->length == 0) {																								// Type list have any entry?
		return Null;																													// No? So don't even lose time searching in it
	}
	
	ListForeach(FsTypeList, i) {
		PFsType typ = (PFsType)(i->data);
		
		if (StrGetLength(typ->name) != StrGetLength(type)) {																			// Same length as the requested type?
			continue;																													// No, so we don't even need to compare
		} else if (StrCompare(typ->name, type)) {																						// We found it?
			return typ;																													// We found it
		}
	}
	
	return Null;																														// We haven't found it
}

Boolean FsAddMountPoint(PChar path, PChar type, PFsNode root) {
	if ((FsMountPointList == Null) || (path == Null) || (type == Null) || (root == Null)) {												// Null pointer checks
		return False;
	}
	
	PChar rpath = Null;
	FsGetMountPoint(path, &rpath);
	UIntPtr len = StrGetLength(path);
	 
	if (path[len - 1] == '\\' && rpath != Null && !StrCompare(rpath, "")) {																// This mount point doesn't exist right?
		return False;																													// ...
	} else if (path[len - 1] != '\\' && rpath != Null && StrCompare(path, "")) {														// Same check
		return False;
	}
	
	PFsMountPoint mp = (PFsMountPoint)MemAllocate(sizeof(FsMountPoint));																// Let's try to allocate space for the struct
	
	if (mp == Null) {
		return False;																													// Failed
	}
	
	mp->path = path;
	mp->type = type;
	mp->root = root;
	
	if (!ListAdd(FsMountPointList, mp)) {																								// Try to add it to the list
		MemFree((UIntPtr)mp);																											// Failed...
		return False;
	}
	
	return True;
}

Boolean FsRemoveMountPoint(PChar path) {
	if ((FsMountPointList == Null) || (path == Null)) {																					// Null pointer(s)?
		return False;																													// Yes
	}
	
	PChar rpath = Null;
	PFsMountPoint mp = FsGetMountPoint(path, &rpath);																					// Let's try to find the mount point
	UIntPtr len = StrGetLength(path);
	
	if ((mp == Null) || (rpath == Null)) {																								// Found it?
		return False;																													// No....
	} else if (path[len - 1] == '\\' && !StrCompare(rpath, "")) {																		// The user tried to remove the mount point using the name of an file/folder that was inside of it?
		return False;																													// Yes...
	} else if (path[len - 1] != '\\' && StrCompare(rpath, "")) {																		// Same check
		return False;
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	
	for (; !found && idx < FsMountPointList->length; idx++) {																			// Let's search for the index of this mount point in the list
		if (ListGet(FsMountPointList, idx) == mp) {
			found = True;
		}
	}
	
	if (!found) {
		return False;																													// ... Really? How it found it earlier but didn't found it now?
	} else if (ListRemove(FsMountPointList, idx) == Null) {																				// Try to remove it!
		return False;																													// IT FAILED WHEN IT WAS REMOVING... REMOVING!
	}
	
	for (idx = 0, found = False; !found && idx < FsMountPointList->length; idx++) {														// Let's try to find it again
		if (ListGet(FsMountPointList, idx) == mp) {
			found = True;
		}
	}
	
	if (found) {																														// Found?
		return False;																													// Yes, so the remove failed
	}
	
	MemFree((UIntPtr)mp->root->name);																									// Free everything
	MemFree((UIntPtr)mp->root);
	MemFree((UIntPtr)mp->type);
	MemFree((UIntPtr)mp->path);
	MemFree((UIntPtr)mp);
	
	return True;																														// And return True!
}

Boolean FsAddType(PChar name, Boolean (*probe)(PFsNode), PFsMountPoint (*mount)(PFsNode, PChar), Boolean (*umount)(PFsMountPoint)) {
	if ((FsTypeList == Null) || (name == Null) || (probe == Null) || (mount == Null) || (umount == Null)) {								// Null pointer checks
		return False;
	} else if (FsGetType(name) != Null) {																								// This fs type doesn't exists... right?
		return False;																													// ...
	}
	
	PFsType type = (PFsType)MemAllocate(sizeof(FsType));																				// Let's try to allocate space for the struct
	
	if (type == Null) {
		return False;																													// Failed
	}
	
	type->name = name;
	type->probe = probe;
	type->mount = mount;
	type->umount = umount;
	
	if (!ListAdd(FsTypeList, type)) {																									// Try to add it to the list
		MemFree((UIntPtr)type);																											// Failed...
		return False;
	}
	
	return True;
}

Boolean FsRemoveType(PChar name) {
	if ((FsTypeList == Null) || (name == Null)) {																						// Null pointer(s)?
		return False;																													// Yes
	}
	
	PFsType type = FsGetType(name);																										// Let's try to find the fs type
	
	if (type == Null) {																													// Found it?
		return False;																													// No...
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	
	for (; !found && idx < FsTypeList->length; idx++) {																					// Let's search for the index of this fs type in the list
		if (ListGet(FsTypeList, idx) == type) {
			found = True;
		}
	}
	
	if (!found) {
		return False;																													// ... Really? How it found it earlier but didn't found it now?
	} else if (ListRemove(FsTypeList, idx) == Null) {																					// Try to remove it!
		return False;																													// IT FAILED WHEN IT WAS REMOVING... REMOVING!
	}
	
	for (idx = 0, found = False; !found && idx < FsTypeList->length; idx++) {															// Let's try to find it again
		if (ListGet(FsTypeList, idx) == type) {
			found = True;
		}
	}
	
	if (found) {																														// Found?
		return False;																													// Yes, so the remove failed
	}
	
	MemFree((UIntPtr)type->name);																										// Free everything
	MemFree((UIntPtr)type);
	
	return True;																														// And return True!
}

Void FsInitTypes(Void) {
	DevFsInit();																														// Add the DevFs to the fs type list
	Iso9660Init();																														// Add the Iso9660 to the fs type list
}

Void FsInit(Void) {
	FsMountPointList = ListNew(True);																									// Let's init our mount point list
	FsTypeList = ListNew(True);																											// And our filesystem type list
	
	if ((FsMountPointList == Null) || (FsTypeList == Null)) {																			// Failed?
		ConWriteFormated("PANIC! Couldn't init mount point or filesystem type list\r\n");
		return;
	}
	
	FsInitTypes();																														// Init all the supported fs types
	
	PChar bdpath = FsJoinPath("\\Devices", FsGetBootDevice());																			// Let's mount the boot device!
	
	if (bdpath == Null) {
		ConWriteFormated("PANIC! Couldn't mount the boot device\r\n");
		ArchHalt();																														// Halt
	}
	
	if (!FsMountFile("\\", bdpath, Null)) {
		ConWriteFormated("PANIC! Couldn't mount the boot device\r\n");
		ArchHalt();																														// Halt
	}
	
	MemFree((UIntPtr)bdpath);
}
