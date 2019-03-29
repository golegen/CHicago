// File author is Ítalo Lima Marconato Matias
//
// Created on March 29 of 2019, at 18:48 BRT
// Last edited on March 29 of 2019, at 19:23 BRT

#include <chicago/alloc.h>
#include <chicago/chexec.h>
#include <chicago/file.h>
#include <chicago/string.h>

static UIntPtr CHExecGetSymbol(PUInt8 buf, PWChar name) {
	PCHExecHeader hdr = (PCHExecHeader)buf;
	PCHExecSymbol sym = (PCHExecSymbol)(((UIntPtr)buf) + hdr->st_start);
	
	for (UIntPtr i = 0; i < hdr->st_count; i++) {																	// Let's do it!
		if (StrGetLengthW(sym->name) != StrGetLengthW(name)) {														// Same length?
			continue;																								// No...
		} else if (StrCompareW(sym->name, name)) {
			return sym->virt;																						// Found!
		}
		
		sym = (PCHExecSymbol)(((UIntPtr)sym) + sizeof(CHExecSymbol) + sym->name_len);
	}
	
	return 0;
}

UIntPtr CHExecLoadKernel(PChar path, UIntPtr addr) {
	PFsNode file = FsOpenFile(path);																				// Try to open the kernel file
	
	if (file == Null) {
		return 0;																									// It doesn't exists, return False
	}
	
	PUInt8 buf = (PUInt8)MemAllocate(file->length);																	// Try to alloc the buffer to read it
	PCHExecHeader hdr = (PCHExecHeader)buf;
	
	if (buf == Null) {
		FsCloseFile(file);																							// Failed
		return 0;
	} else if (!FsReadFile(file, 0, file->length, buf)) {															// Read it!
		MemFree((UIntPtr)buf);																						// Failed
		FsCloseFile(file);
		return 0;
	}
	
	FsCloseFile(file);																								// Ok, now we can close the file!
	
	UIntPtr entry = hdr->entry;																						// Save the entry point
	
	if (hdr->magic != CHEXEC_HEADER_MAGIC) {																		// Valid magic?
		MemFree((UIntPtr)buf);																						// No...
		return 0;
	} else if ((hdr->flags & CHEXEC_HEADER_FLAGS_ARCH_X86) != CHEXEC_HEADER_FLAGS_ARCH_X86) {						// Valid arch?
		MemFree((UIntPtr)buf);																						// No...
		return 0;
	} else if ((hdr->flags & CHEXEC_HEADER_FLAGS_KERNEL) != CHEXEC_HEADER_FLAGS_KERNEL) {							// Valid type?
		MemFree((UIntPtr)buf);																						// No...
		return 0;
	}
	
	PCHExecSection sh = (PCHExecSection)(((UIntPtr)buf) + hdr->sh_start);											// Let's load all the sections
	
	for (UIntPtr i = 0; i < hdr->sh_count; i++) {
		if ((sh->flags & CHEXEC_SECTION_FLAGS_ZEROINIT) == CHEXEC_SECTION_FLAGS_ZEROINIT) {							// BSS?
			StrSetMemory((PUInt8)(sh->virt - addr), 0, sh->size);													// Yes, just zero it
		} else {
			StrCopyMemory((PUInt8)(sh->virt - addr), (PUInt8)(((UIntPtr)buf) + sh->offset), sh->size);				// No, copy it!
		}
		
		sh = (PCHExecSection)(((UIntPtr)sh) + sizeof(CHExecSection) + (sh->name_len * sizeof(WChar)));
	}
	
	PCHExecRelocation rel = (PCHExecRelocation)(((UIntPtr)buf) + hdr->rel_start);									// And let's relocate everything (that we need to, of course!)
	
	for (UIntPtr i = 0; i < hdr->rel_count; i++) {
		UIntPtr incr = rel->incr;
		
		if ((rel->op & CHEXEC_REL_OP_REL) == CHEXEC_REL_OP_REL) {													// Relative
			incr -= rel->virt;
		} else if ((rel->op & CHEXEC_REL_OP_SYM) == CHEXEC_REL_OP_SYM) {											// Symbol
			UIntPtr sym = CHExecGetSymbol(buf, rel->name);															// Try to get the symbol
			
			if (sym == 0) {
				MemFree((UIntPtr)buf);																				// Failed
				return 0;
			}
			
			incr += sym;
		} else if ((rel->op & CHEXEC_REL_OP_REL_SYM) == CHEXEC_REL_OP_REL_SYM) {									// Relative symbol
			UIntPtr sym = CHExecGetSymbol(buf, rel->name);															// Try to get the symbol
			
			if (sym == 0) {
				MemFree((UIntPtr)buf);																				// Failed
				return 0;			
			}
			
			incr += sym - rel->virt;
		}
		
		if ((rel->op & CHEXEC_REL_OP_BYTE) == CHEXEC_REL_OP_BYTE) {													// Byte
			*((PUInt8)(rel->virt - addr)) += (UInt8)incr;
		} else if ((rel->op & CHEXEC_REL_OP_WORD) == CHEXEC_REL_OP_WORD) {											// Word
			*((PUInt16)(rel->virt - addr)) += (UInt16)incr;
		} else if ((rel->op & CHEXEC_REL_OP_DWORD) == CHEXEC_REL_OP_DWORD) {										// DWord
			*((PUInt32)(rel->virt - addr)) += (UInt32)incr;
		}
		
		rel = (PCHExecRelocation)(((UIntPtr)rel) + sizeof(CHExecRelocation) + (rel->name_len * sizeof(WChar)));
	}
	
	MemFree((UIntPtr)buf);																							// Free the buffer
	
	return entry;
}
