// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 14:29 BRT
// Last edited on October 27 of 2018, at 12:55 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/config.h>
#include <chicago/console.h>
#include <chicago/elf.h>
#include <chicago/file.h>
#include <chicago/keyboard.h>
#include <chicago/list.h>
#include <chicago/menu.h>
#include <chicago/string.h>
#include <chicago/version.h>

PList MenuOptions = Null;
PChar MenuDefault = Null;
UIntPtr MenuSelectedOption = 0;

static Boolean MenuCompareStrings(PChar a, PChar b) {
	if (a == Null && b == Null) {																			// Both are Null?
		return True;																						// Yes
	} else if (a == Null || b == Null) {																	// One is Null and the other one isn't?
		return False;																						// Yes
	} else if (StrGetLength(a) != StrGetLength(b)) {														// Same length?
		return False;																						// Nope
	}
	
	return StrCompare(a, b);																				// Compare!
}

static PMenuOption MenuGetOption(PChar name) {
	if (name == Null) {																						// Sanity check
		return Null;
	}
	
	ListForeach(MenuOptions, i) {																			// Let's search!
		PMenuOption opt = (PMenuOption)i->data;
		
		if (MenuCompareStrings(name, opt->name)) {															// Found?
			return opt;																						// Yes!
		}
	}
	
	return Null;																							// Not found...
}

static UIntPtr MenuGetOptionIdx(PChar name) {
	if (name == Null) {																						// Sanity check
		return 0;
	}
	
	UIntPtr idx = 0;
	
	ListForeach(MenuOptions, i) {																			// Let's search!
		PMenuOption opt = (PMenuOption)i->data;
		
		if (MenuCompareStrings(name, opt->name)) {															// Found?
			return idx;																						// Yes!
		} else {
			idx++;
		}
	}
	
	return 0;																								// Not found...
}

static Void MenuIterator(PConfField field) {
	if (MenuCompareStrings(field->name, "default")) {														// Is the set default entry command?
		MenuDefault = field->value;																			// Yes
		return;
	}
	
	PMenuOption opt = MenuGetOption(field->name);															// Let's see if this option already exists
	Boolean add = True;
	
	if (opt == Null) {
		opt = (PMenuOption)MemAllocate(sizeof(MenuOption));													// Nope, it doesn't, so let's alloc some space
	} else {
		add = False;																						// Yes, it exists!
	}
	
	if (opt == Null) {
		ConWriteFormated("PANIC! Couldn't init the menu\r\n");												// Failed to alloc...
		while (1) ;
	}
	
	opt->name = field->name;																				// Set the name
	opt->device = field->value;																				// Set the device
	
	if (field->attrs == Null) {
		ConWriteFormated("PANIC! Couldn't init the menu\r\n");												// No attributes...
		while (1) ;
	}
	
	opt->boot_type = (PChar)ListGet(field->attrs, field->attrs->length - 1);								// Set the boot type!
	
	if (add) {
		if (!ListAdd(MenuOptions, opt)) {																	// Add to the menu option list!
			ConWriteFormated("PANIC! Couldn't init the menu\r\n");											// Failed...
			while (1) ;
		}
	}
}

static Void MenuShowMessage(PChar msg, PChar msg2) {
	if (msg == Null) {																						// Valid message?
		return;																								// Nope...
	}
	
	while (1) {
		ConClearScreen();																					// Clear the screen
		ConWriteFormated("%s", msg);																		// Print the message
		
		if (msg2 != Null) {
			ConWriteFormated(" '%s'", msg2);
		}
		
		ConWriteFormated("\r\n\r\n");
		ConSetColor(CON_COLOR_INVERSE);
		ConWriteFormated("    OK\r\n");																		// Print the avaliable option, the "OK" option
		ConSetColor(CON_COLOR_NORMAL);
		
		UInt8 key = KbdReadKey();																			// Wait for key
		
		if (key == 0x1C) {																					// Enter
			break;
		}
	}
}

Void MenuLoop(Void) {
	ConHideCursor();																						// Hide the cursor
	
	MenuSelectedOption = MenuGetOptionIdx(MenuDefault);														// Select the default option
	
l:	while (1) {
		if (MenuSelectedOption >= MenuOptions->length) {													// Fix the current selected option
			MenuSelectedOption = MenuOptions->length - 1;
		}
		
		ConClearScreen();																					// Clear the screen
		ConWriteFormated("CHicago Boot Manager\r\n%s\r\n\r\n", CHICAGO_VSTR);								// Print the banner
		
		UIntPtr idx = 0;
		
		ListForeach(MenuOptions, i) {																		// Print all the options
			PMenuOption opt = (PMenuOption)i->data;
			
			if (idx == MenuSelectedOption) {																// Selected one?
				ConSetColor(CON_COLOR_INVERSE);																// Yes, print with other color
			}
			
			ConWriteFormated("    %s\r\n", opt->name);
			
			if (idx++ == MenuSelectedOption) {
				ConSetColor(CON_COLOR_NORMAL);																// Revert back to the normal color
			}
			
			if (i->next == Null) {
				ConWriteFormated("\r\n");
			}
		}
		
		ConWriteFormated("Select an option with the arrow keys\r\n");										// Print the footer
		ConWriteFormated("Press Enter to select the highlighted option\r\n");
		
		UInt8 key = KbdReadKey();																			// Wait for keypress
		
		if (key == 0x1C) {																					// Enter
			break;
		} else if (key == 0x48) {																			// Key down
			MenuSelectedOption--;
		} else if (key == 0x50) {																			// Key up
			MenuSelectedOption++;
		}
	}
	
	PMenuOption opt = (PMenuOption)ListGet(MenuOptions, MenuSelectedOption);								// Get the selected option
	Boolean rootdev = False;
	PFsNode root = Null;
	
	if (MenuCompareStrings(opt->device, "BootDevice")) {													// The device is the boot device?
		rootdev = True;																						// Yes!
	} else {
		root = FsOpenFile(opt->device);																		// No, so let's (try to) open the device
	}
	
	if (!rootdev) {
		if (root == Null) {																					// Failed?
			MenuShowMessage("Invalid boot device", opt->device);											// Yes, show error message
			goto l;																							// And go back to the menu loop
		}
		
		if (MenuCompareStrings(root->name, FsGetBootDevice())) {											// Root device == Boot device?
			FsCloseFile(root);																				// Yes, so we don't need to mount it!
			rootdev = True;
		} else if (!FsMountFile("\\RootDevice", opt->device, Null)) {										// Mount the root device!
			FsCloseFile(root);																				// Failed...
			MenuShowMessage("Couldn't mount the root device, please try again", Null);
			goto l;
		}
	}
	
	if (MenuCompareStrings(opt->boot_type, "chicago")) {
		PFsNode file = Null;																				// Open the kernel file
		
		if (rootdev) {
			file = FsOpenFile("\\Boot\\chkrnl.elf");
		} else {
			file = FsOpenFile("\\RootDevice\\Boot\\chkrnl.elf");
		}
		
		if (file == Null) {
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowMessage("Couldn't find the CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		}
		
		PUInt8 buffer = (PUInt8)MemAllocate(file->length);													// Allocate space for reading the file
		
		if (buffer == Null) {
			ConClearScreen();																				// Failed...
			ConWriteFormated("PANIC! Couldn't allocate memory for reading the kernel\r\n");	
			while (1) ;
		}
		
		if (!FsReadFile(file, 0, file->length, buffer)) {													// Read the file!
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowMessage("Couldn't read the CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		}
		
		FsCloseFile(file);																					// Close the file
		
		PELFHeader hdr = (PELFHeader)buffer;
		
		if (!StrCompareMemory(hdr->ident, "\177ELF", 4)) {													// ELF file?
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		} else if (hdr->type != 2) {																		// Executable?
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		} else if (hdr->machine != ELF_MACHINE) {															// Valid machine?
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		} else if (hdr->version == 0) {																		// Valid version?
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		}
		
		for (UIntPtr i = 0; i < hdr->ph_num; i++) {															// Load the file!
			PELFProgramHeader ph = (PELFProgramHeader)(buffer + hdr->ph_off + i * hdr->ph_ent_size);
			
			StrCopyMemory((PVoid)(ph->vaddr - ELF_BASE), buffer + ph->offset, ph->fsize);
			StrSetMemory((PVoid)(ph->vaddr - ELF_BASE + ph->fsize), 0, ph->msize - ph->fsize);
		}
		
		MemFree((UIntPtr)buffer);																			// Free the buffer
		
		IntPtr err = ArchJump(hdr->entry - ELF_BASE, rootdev ? FsGetBootDevice() : root->name);				// Try to jump!
		
		if (err == -1) {
			if (!rootdev) {																					// Failed (-1)
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowMessage("Couldn't jump to the kernel, please try again", Null);							// Show the error message
			goto l;
		} else if (err == -2) {
			if (!rootdev) {																					// Failed (-2)
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowMessage("Your graphic card doesn't support 800x600x32 mode", Null);
			goto l;
		}
	} else {
		MenuShowMessage("Invalid boot type", opt->boot_type);												// Invalid boot type, show error message
		FsCloseFile(root);																					// Close the "root file"
		goto l;																								// And go back to the menu loop
	}
}

Void MenuInit(Void) {
	MenuOptions = ListNew(False);																			// Alloc the option list
	
	if (MenuOptions == Null) {
		ConWriteFormated("PANIC! Failed to init the menu\r\n");												// Failed
		while (1) ;
	}
	
	ConfIterate(MenuIterator);																				// Init the option list
}
