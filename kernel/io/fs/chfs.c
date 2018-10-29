// File author is Ítalo Lima Marconato Matias
//
// Created on October 28 of 2018, at 09:41 BRT
// Last edited on October 29 of 2018, at 15:19 BRT

#include <chicago/alloc.h>
#include <chicago/chfs.h>
#include <chicago/file.h>
#include <chicago/string.h>

Boolean CHFsReadFile(PFsNode file, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (file == Null) {																						// Let's do some null pointer checks first!
		return False;
	} else if ((file->priv == Null) || (file->inode == 0)) {
		return False;
	} else if (file->read == Null) {
		return False;
	} else if ((file->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {												// We're trying to read raw bytes from an directory?
		return False;																						// Why?
	} else if (off >= file->length) {																		// For byte per byte read
		return False;
	}
	
	PFsNode dev = file->priv;																				// Let's get our base device (it's inside of the priv)
	
	if (dev->read == Null) {																				// We have the read function... right?
		return False;																						// Nope (how you initialized this device WITHOUT THE READ FUNCTION????????????)
	} if ((dev->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {													// It's a file?
		return False;																						// Nope (again, how?)
	}
	
	PCHFsINode inode = (PCHFsINode)file->inode;																// Get the inode
	PUInt8 buff = (PUInt8)MemAllocate(512);																	// Alloc some memory for reading the disk
	UInt32 lba = inode->data_start;
	UIntPtr cur = off;
	UIntPtr end = 0;
	
	if (buff == Null) {
		return False;																						// ...
	} else if ((off + len) > file->length) {																// Let's calc the size that we're going to read
		end = file->length;
	} else {
		end = off + len;
	}
	
	while (cur < end) {																						// And read the file!
		if (!dev->read(dev, lba * 512, 512, buff)) {														// Read the sector
			MemFree((UIntPtr)buff);																			// Failed...
			return False;
		}
		
		if (cur - end >= 508) {																				// Our data block is 508-bytes long (would be 512, but 4-bytes are reserved for the next chain block num), we all the 508 bytes?
			StrCopyMemory(buf + cur, buff + 4, 508);														// Yes
			cur -= 508;
		} else {
			StrCopyMemory(buf + cur, buf + 4, cur - end);													// No, so only read what we need
			cur -= cur - end;
		}
		
		lba = *((PUInt32)buff);																				// Get the next block from the chain
	}
	
	MemFree((UIntPtr)buff);
	
	return True;
}

Boolean CHFsOpenFile(PFsNode node) {
	if (node == Null) {																						// Trying to open an null pointer?
		return False;																						// Yes (probably someone called this function directly... or the kernel have some fatal bug)
	} else {
		return True;
	}
}

Void CHFsCloseFile(PFsNode node) {
	if (node == Null) {																						// Null pointer?
		return;																								// Yes
	} else if (StrCompare(node->name, "\\")) {																// Root directory?
		return;																								// Yes, don't free it, it's important for us (only the umount function can free it)
	}
	
	MemFree((UIntPtr)node->name);																			// Let's free everything that we allocated!
	MemFree(node->inode);
	MemFree((UIntPtr)node);
}

PChar CHFsReadDirectoryEntry(PFsNode dir, UIntPtr entry) {
	if (dir == Null) {																						// Let's do some null pointer checks first!
		return Null;
	} else if ((dir->priv == Null) || (dir->inode == 0)) {
		return Null;
	} else if ((dir->flags & FS_FLAG_DIR) != FS_FLAG_DIR) {													// Trying to read an directory entry using an file... why?
		return Null;
	} else if (entry == 0) {																				// Current directory?
		return StrDuplicate(".");
	} else if (entry == 1) {																				// Parent directory?
		return StrDuplicate("..");
	}
	
	PFsNode dev = dir->priv;																				// Get our device and make sure that it's valid
	
	if (dev->read == Null) {
		return Null;
	} if ((dev->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {
		return Null;
	}
	
	PCHFsINode inode = (PCHFsINode)dir->inode;																// Get the inode
	PUInt8 buff = (PUInt8)MemAllocate(512);																	// Alloc some memory for reading the disk
	PCHFsINode ent = (PCHFsINode)buff;
	UInt32 lba = inode->data_start;
	
	if (buff == Null) {
		return Null;																						// ...
	}
	
	for (UIntPtr i = 0;; i++) {																				// Let's search for the entry!
		if (!dev->read(dev, lba * 512, 512, buff)) {														// Read the sector
			MemFree((UIntPtr)buff);																			// Failed...
			return Null;
		} else if (ent->type == 0x00) {																		// End of the directory?
			MemFree((UIntPtr)buff);																			// Yes :(
			return Null;
		} else if (i == entry - 2) {
			UIntPtr nlen = ent->name_length;																// Save the name length
			PChar ret = (PChar)MemAllocate(nlen + 1);														// Alloc space for it
			
			if (ret == Null) {
				MemFree((UIntPtr)buff);																		// :(
				return Null;
			}
			
			StrCopyMemory(ret, (PVoid)(&ent->name), nlen);													// Copy it
			MemFree((UIntPtr)buff);																			// Free the buffer
			
			ret[nlen] = '\0';																				// End the return string with a NUL character
			
			return ret;
		}
		
		lba = ent->next_block;																				// Follow to the next block
	}
	
	MemFree((UIntPtr)buff);																					// The function should end on ent->type == 0x00, but if we got here, free the buffer and return Null
	return Null;
}

PFsNode CHFsFindInDirectory(PFsNode dir, PChar name) {
	if ((dir == Null) || (name == Null)) {																	// Let's do some null pointer checks first!
		return Null;
	} else if ((dir->priv == Null) || (dir->inode == 0)) {
		return Null;
	} else if ((dir->flags & FS_FLAG_DIR) != FS_FLAG_DIR) {													// Trying to read an directory entry using an file... why?
		return Null;
	}
	
	PCHFsINode inode = (PCHFsINode)dir->inode;																// Get the inode
	PUInt8 buff = (PUInt8)MemAllocate(512);																	// Alloc some memory for reading the disk
	PCHFsINode ent = (PCHFsINode)buff;
	UInt32 lba = inode->data_start;
	
	if (buff == Null) {
		return Null;																						// ...
	}
	
	PFsNode dev = dir->priv;																				// Get our device and make sure that it's valid
	
	if (dev->read == Null) {
		return Null;
	} if ((dev->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {
		return Null;
	}
	
	for (UIntPtr i = 0;; i++) {																				// Let's search for the entry!
		if (!dev->read(dev, lba * 512, 512, buff)) {														// Read the sector
			MemFree((UIntPtr)buff);																			// Failed...
			return Null;
		} else if (ent->type == 0x00) {																		// End of the directory?
			MemFree((UIntPtr)buff);																			// Yes :(
			return Null;
		}
		
		PChar entname = (PChar)MemAllocate(ent->name_length + 1);											// Alloc space for copying the name of this entry
		
		if (entname == Null) {
			MemFree((UIntPtr)buff);																			// Failed...
			return Null;
		}
		
		StrCopyMemory(entname, (PVoid)(&ent->name), ent->name_length);										// Copy the name
		entname[ent->name_length] = '\0';																	// End it with a NUL character
		
		if (StrGetLength(entname) == StrGetLength(name)) {													// same length?
			if (StrCompare(entname, name)) {																// SAME NAME?
				PCHFsINode ino = (PCHFsINode)MemAllocate(sizeof(CHFsINode) + ent->name_length);				// Yes! Let's try to alloc space for copying the inode
				
				if (ino == Null) {
					MemFree((UIntPtr)entname);																// ...
					MemFree((UIntPtr)buff);
					return Null;
				} else {
					StrCopyMemory(ino, ent, sizeof(CHFsINode) + ent->name_length);							// Copy the inode!
				}
				
				PFsNode node = (PFsNode)MemAllocate(sizeof(FsNode));										// Try to alloc space for the file node itself
				
				if (node == Null) {
					MemFree((UIntPtr)ino);																	// ...
					MemFree((UIntPtr)entname);
					MemFree((UIntPtr)buff);
					return Null;
				}
				
				node->name = entname;																		// Set the name
				node->priv = dev;																			// The priv (device)
				
				if (ent->type == 0x01) {																	// File?
					node->flags = FS_FLAG_FILE;																// Yes, set the file flag and the read function (for now)
					node->read = CHFsReadFile;
					node->write = Null;
					node->readdir = Null;
					node->finddir = Null;
				} else {
					node->flags = FS_FLAG_DIR;																// No, set the dir flag, the readdir entry and the finddir entry
					node->read = Null;
					node->write = Null;
					node->readdir = CHFsReadDirectoryEntry;
					node->finddir = CHFsFindInDirectory;
				}
				
				node->inode = (UIntPtr)ino;																	// Set the inode
				node->length = ent->data_length;															// The length
				node->open = CHFsOpenFile;																	// The open and close function
				node->close = CHFsCloseFile;
				
				MemFree((UIntPtr)buff);																		// Free the buffer
				
				return node;																				// And return :)
			}
		}
		
		MemFree((UIntPtr)entname);																			// Free the copied name
		
		lba = ent->next_block;																				// Follow to the next block
	}
	
	MemFree((UIntPtr)buff);																					// The function should end on ent->type == 0x00, but if we got here, free the buffer and return Null
	return Null;
}

Boolean CHFsProbe(PFsNode file) {
	if (file == Null) {																						// Sanity checks :)
		return False;
	} else if ((file->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {
		return False;
	} else if (file->read == Null) {
		return False;
	}
	
	PCHFsHeader hdr = (PCHFsHeader)MemAllocate(sizeof(CHFsHeader));											// Alloc space for reading the header
	
	if (hdr == Null) {
		return False;
	}
	
	if (!file->read(file, 0, 512, (PUInt8)hdr)) {															// Read it!
		MemFree((UIntPtr)hdr);
		return False;
	} else if (StrCompareMemory(hdr->magic, "CHFS", 4)) {													// Check the magic number
		MemFree((UIntPtr)hdr);																				// :)
		return True;
	}
	
	MemFree((UIntPtr)hdr);																					// Free the header
	
	return False;
}

PFsMountPoint CHFsMount(PFsNode file, PChar path) {
	if ((file == Null) || (path == Null)) {																	// Sanity checks :)
		return False;
	} else if ((file->flags & FS_FLAG_FILE) != FS_FLAG_FILE) {
		return False;
	} else if (file->read == Null) {
		return False;
	}
	
	PUInt8 buf = (PUInt8)MemAllocate(512);																	// Alloc space for reading from the disk
	
	if (buf == Null) {
		return Null;
	}
	
	PCHFsHeader hdr = (PCHFsHeader)buf;
	
	if (!file->read(file, 0, 512, buf)) {																	// Read it!
		MemFree((UIntPtr)buf);
		return Null;
	} else if (!StrCompareMemory(hdr->magic, "CHFS", 4)) {													// And check the magic number/signature
		MemFree((UIntPtr)buf);
		return Null;
	}
	
	PCHFsINode inode = (PCHFsINode)MemAllocate(11);															// Alloc space for the root inode
	
	if (inode == Null) {
		MemFree((UIntPtr)buf);
		return Null;
	} else if (!file->read(file, 0, hdr->root_directory_start * 512, buf)) {								// Read it
		MemFree((UIntPtr)inode);
		MemFree((UIntPtr)buf);
		return Null;
	} else {
		StrCopyMemory(inode, buf, 11);																		// Copy it
		MemFree((UIntPtr)buf);																				// And free the buffer!
	}
	
	PFsMountPoint mp = (PFsMountPoint)MemAllocate(sizeof(FsMountPoint));									// Let's alloc space for the mount point struct
	
	if (mp == Null) {
		MemFree((UIntPtr)inode);
		return Null;
	}
	
	mp->path = StrDuplicate(path);																			// Let's try to duplicate the path string
	
	if (mp->path == Null) {
		MemFree((UIntPtr)mp);
		MemFree((UIntPtr)inode);
		return Null;
	}
	
	mp->type = StrDuplicate("CHFs");																		// And the type string
	
	if (mp->type == Null) {
		MemFree((UIntPtr)mp->path);
		MemFree((UIntPtr)mp);
		MemFree((UIntPtr)inode);
		return Null;
	}
	
	mp->root = (PFsNode)MemAllocate(sizeof(FsNode));														// Create the root directory node
	
	if (mp->root == Null) {
		MemFree((UIntPtr)mp->type);
		MemFree((UIntPtr)mp->path);
		MemFree((UIntPtr)mp);
		MemFree((UIntPtr)inode);
		return Null;
	}
	
	mp->root->name = StrDuplicate("\\");																	// Duplicate the name
	
	if (mp->root->name == Null) {
		MemFree((UIntPtr)mp->root);
		MemFree((UIntPtr)mp->type);
		MemFree((UIntPtr)mp->path);
		MemFree((UIntPtr)mp);
		MemFree((UIntPtr)inode);
		return Null;
	}
	
	mp->root->priv = file;																					// Finally, fill everything!
	mp->root->flags = FS_FLAG_DIR;
	mp->root->inode = (UIntPtr)inode;
	mp->root->length = 0;
	mp->root->read = Null;
	mp->root->write = Null;
	mp->root->open = CHFsOpenFile;
	mp->root->close = CHFsCloseFile;
	mp->root->readdir = CHFsReadDirectoryEntry;
	mp->root->finddir = CHFsFindInDirectory;
	
	return mp;
}

Boolean CHFsUmount(PFsMountPoint mp) {
	if (mp == Null) {																						// Sanity checks
		return False;
	} else if (mp->root == Null) {
		return False;
	} else if (mp->root->priv == Null) {
		return False;
	} else if (mp->root->inode == 0) {
		return False;
	} else if (!StrCompare(mp->type, "CHFs")) {
		return False;
	}
	
	FsCloseFile((PFsNode)mp->root->priv);																	// Close the dev file
	MemFree(mp->root->inode);																				// And free the root inode
	
	return True;
}

Void CHFsInit(Void) {
	PChar name = StrDuplicate("CHFs");																		// Let's get our type string
	
	if (name == Null) {
		return;																								// Failed...
	}
	
	FsAddType(name, CHFsProbe, CHFsMount, CHFsUmount);														// And add ourself into the type list
}
