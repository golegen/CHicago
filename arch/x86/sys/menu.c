// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 14:29 BRT
// Last edited on May 01 of 2019, at 12:19 BRT

#define StrDuplicate dummy

#include <efi.h>
#include <efilib.h>

#undef StrDuplicate

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/chexec.h>
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
		ArchHalt();																							// Halt
	}
	
	opt->name = field->name;																				// Set the name
	opt->device = field->value;																				// Set the device
	
	if (field->attrs == Null) {
		ConWriteFormated("PANIC! Couldn't init the menu\r\n");												// No attributes...
		ArchHalt();																							// Halt
	}
	
	ListForeach(field->attrs, i) {
		opt->boot_type = (PChar)i->data;																	// Set the boot type
	}
	
	if (add) {
		if (!ListAdd(MenuOptions, opt)) {																	// Add to the menu option list!
			ConWriteFormated("PANIC! Couldn't init the menu\r\n");											// Failed...
			ArchHalt();																						// Halt
		}
	}
}

static Void MenuShowOkMessage(PChar msg, PChar msg2) {
	if (msg == Null) {																						// Valid message?
		return;																								// Nope...
	}
	
	while (True) {
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

static Void MenuPrintOption(UIntPtr idx, UIntPtr sel, Boolean last, PChar text) {
	if (idx == sel) {																						// Selected one?
		ConSetColor(CON_COLOR_INVERSE);																		// Yes, print with other color
	}
	
	ConWriteFormated("    %s\r\n", text);
	
	if (idx == sel) {
		ConSetColor(CON_COLOR_NORMAL);																		// Revert back to the normal color
	}
	
	if (last) {
		ConWriteFormated("\r\n");
	}
}

static Boolean MenuAdvancedOptions(PUIntPtr ret, Boolean agb) {
	PMenuOption opt = (PMenuOption)ListGet(MenuOptions, MenuSelectedOption);								// Get the selected option
	PChar opttype = opt->boot_type;
	
	if (MenuCompareStrings(opttype, "chicago") || MenuCompareStrings(opttype, "chicago-old")) {				// CHicago
		UIntPtr cur = 0;
		UIntPtr max = agb ? 2 : 1;
		
		while (True) {																						// Let's go!
			if (cur > max) {																				// Fix the current selected option
				cur = 0;
			}
			
			ConClearScreen();																				// Clear the screen
			ConWriteFormated("CHicago Boot Manager\r\n");													// Print the banner
			ConWriteFormated("Version %d.%d.%d\r\n", CHICAGO_MAJOR, CHICAGO_MINOR, CHICAGO_BUILD);
			ConWriteFormated("Advanced boot options for CHicago\r\n\r\n");
			MenuPrintOption(0, cur, False, "Verbose Boot");													// Print the options
			MenuPrintOption(1, cur, !agb, "Normal Boot");
			
			if (agb) {
				MenuPrintOption(2, cur, True, "Go Back");
			}
			
			ConWriteFormated("Select an option with the arrow keys\r\n");									// Print the footer
			ConWriteFormated("Press Enter to select the highlighted option\r\n");
			
			UInt8 key = KbdReadKey();																		// Wait for keypress
			
			if (key == 0x1C) {																				// Enter
				break;
			} else if (key == 0x48) {																		// Key down
				cur--;
			} else if (key == 0x50) {																		// Key up
				cur++;
			}
		}
		
		if (cur == 0 || cur == 1) {																			// Verbose/normal boot?
			*ret = cur == 0;																				// Yes :)
			return True;
		} else {
			return False;																					// Nope, just return to the boot menu
		}
	} else {
		MenuShowOkMessage("Invalid boot type", opttype);													// Invalid boot type, show error message
		return False;
	}
}

Void MenuLoop(Void) {
	ConHideCursor();																						// Hide the cursor
	
	MenuSelectedOption = MenuGetOptionIdx(MenuDefault);														// Select the default option
	
	UIntPtr options = 0;
	
	if (MenuOptions->length == 1) {																			// Only one option?
		UIntPtr idx;
		EFI_EVENT tevnt;
		EFI_INPUT_KEY key;
		EFI_EVENT evnts[2];
		EFI_STATUS status = BS->CreateEvent(EFI_EVENT_TIMER, 0, Null, Null, &tevnt);						// Yes, create a timer event
		
		if (EFI_ERROR(status)) {
			goto e;																							// ...
		}
		
		status = BS->SetTimer(tevnt, TimerRelative, 1000000);												// Set the timer to 100ms of expiration
		
		if (EFI_ERROR(status)) {
			goto e;																							// ...
		}
		
		evnts[0] = ST->ConIn->WaitForKey;																	// Setup the event list
		evnts[1] = tevnt;
		
		status = BS->WaitForEvent(2, evnts, &idx);															// Wait for the first event that we get
		
		BS->CloseEvent(tevnt);																				// Close the timer event
		ST->ConIn->ReadKeyStroke(ST->ConIn, &key);															// Read the keystroke
		
		if (!EFI_ERROR(status) && idx != 1 && key.ScanCode == SCAN_F8) {									// F8 detected?
			if (!MenuAdvancedOptions(&options, False)) {													// Yes, show the advanced boot options menu
				goto l;
			}
		}
		
		goto e;
	}
	
l:	while (True) {
		if (MenuSelectedOption >= MenuOptions->length) {													// Fix the current selected option
			MenuSelectedOption = 0;
		}
		
		ConClearScreen();																					// Clear the screen
		ConWriteFormated("CHicago Boot Manager\r\n");														// Print the banner
		ConWriteFormated("Version %d.%d.%d\r\n\r\n", CHICAGO_MAJOR, CHICAGO_MINOR, CHICAGO_BUILD);
		
		UIntPtr idx = 0;
		
		ListForeach(MenuOptions, i) {																		// Print all the options
			PMenuOption opt = (PMenuOption)i->data;
			MenuPrintOption(idx++, MenuSelectedOption, i->next == Null, opt->name);
		}
		
		ConWriteFormated("Select an option with the arrow keys\r\n");										// Print the footer
		ConWriteFormated("Press Enter to select the highlighted option\r\n");
		ConWriteFormated("Press Tab for advanced startup options for the highlighted option\r\n");
		
		UInt8 key = KbdReadKey();																			// Wait for keypress
		
		if (key == 0x0F) {																					// Tab
			if (MenuAdvancedOptions(&options, True)) {
				break;
			}
		} else if (key == 0x1C) {																			// Enter
			break;
		} else if (key == 0x48) {																			// Key down
			MenuSelectedOption--;
		} else if (key == 0x50) {																			// Key up
			MenuSelectedOption++;
		}
	}
	
e:	;PMenuOption opt = (PMenuOption)ListGet(MenuOptions, MenuSelectedOption);								// Get the selected option
	Boolean rootdev = False;
	PFsNode root = Null;
	
	if (MenuCompareStrings(opt->device, "BootDevice")) {													// The device is the boot device?
		rootdev = True;																						// Yes!
	} else {
		root = FsOpenFile(opt->device);																		// No, so let's (try to) open the device
	}
	
	if (!rootdev) {
		if (root == Null) {																					// Failed?
			MenuShowOkMessage("Invalid boot device", opt->device);											// Yes, show error message
			goto l;																							// And go back to the menu loop
		}
		
		if (MenuCompareStrings(root->name, FsGetBootDevice())) {											// Root device == Boot device?
			FsCloseFile(root);																				// Yes, so we don't need to mount it!
			rootdev = True;
		} else if (!FsMountFile("\\RootDevice", opt->device, Null)) {										// Mount the root device!
			FsCloseFile(root);																				// Failed...
			MenuShowOkMessage("Couldn't mount the root device, please try again", Null);
			goto l;
		}
	}
	
	if (MenuCompareStrings(opt->boot_type, "chicago")) {													// CHicago (in CHExec format)
		UIntPtr entry = False;																				// Let's load the kernel
		
		if (rootdev) {
			entry = CHExecLoadKernel("\\Boot\\chkrnl.che", 0xC0000000);
		} else {
			entry = CHExecLoadKernel("\\RootDevice\\Boot\\chkrnl.che", 0xC0000000);
		}
		
		if (!entry) {
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowOkMessage("Couldn't load the CHicago kernel (Boot\\chkrnl.che)", Null);
			goto l;
		}
		
		PChar bootdev = rootdev ? FsGetBootDevice() : root->name;											// Get the boot device
		
		if (((UIntPtr)bootdev) >= 0x800000) {																// Realloc it?
			PChar new = (PChar)(0x10000 - (StrGetLength(bootdev) + 1));										// Yes :) Realloc it to "low" memory
			StrCopy(new, bootdev);
			bootdev = new;																					// And set
		}
		
		IntPtr err = ArchJump(entry - 0xC0000000, bootdev, options);										// Try to jump!
		
		if (err == -1) {
			if (!rootdev) {																					// Failed (-1)
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowOkMessage("Couldn't jump to the kernel, please try again", Null);						// Show the error message
			goto l;
		} else if (err == -2) {
			if (!rootdev) {																					// Failed (-2)
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowOkMessage("Your graphic card doesn't support any supported mode", Null);
			goto l;
		}
	} else if (MenuCompareStrings(opt->boot_type, "chicago-old")) {											// CHicago (in ELF format)
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
			
			MenuShowOkMessage("Couldn't find the CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		}
		
		PUInt8 buffer = (PUInt8)MemAllocate(file->length);													// Allocate space for reading the file
		
		if (buffer == Null) {
			ConClearScreen();																				// Failed...
			ConWriteFormated("PANIC! Couldn't allocate memory for reading the kernel\r\n");	
			ArchHalt();																						// Halt
		}
		
		if (!FsReadFile(file, 0, file->length, buffer)) {													// Read the file!
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowOkMessage("Couldn't read the CHicago kernel (Boot\\chkrnl.elf)", Null);
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
			MenuShowOkMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		} else if (hdr->type != 2) {																		// Executable?
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowOkMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		} else if (hdr->machine != ELF_MACHINE) {															// Valid machine?
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowOkMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		} else if (hdr->version == 0) {																		// Valid version?
			if (!rootdev) {																					// Failed...
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MemFree((UIntPtr)buffer);
			MenuShowOkMessage("Invalid CHicago kernel (Boot\\chkrnl.elf)", Null);
			goto l;
		}
		
		for (UIntPtr i = 0; i < hdr->ph_num; i++) {															// Load the file!
			PELFProgramHeader ph = (PELFProgramHeader)(buffer + hdr->ph_off + i * hdr->ph_ent_size);
			
			StrCopyMemory((PVoid)((UIntPtr)(ph->vaddr - 0xC0000000)), buffer + ph->offset, ph->fsize);
			StrSetMemory((PVoid)((UIntPtr)(ph->vaddr - 0xC0000000 + ph->fsize)), 0, ph->msize - ph->fsize);
		}
		
		PChar bootdev = rootdev ? FsGetBootDevice() : root->name;											// Get the boot device
		
		if (((UIntPtr)bootdev) >= 0x800000) {																// Realloc it?
			PChar new = (PChar)(0x10000 - (StrGetLength(bootdev) + 1));										// Yes :) Realloc it to "low" memory
			StrCopy(new, bootdev);
			bootdev = new;																					// And set
		}
		
		IntPtr err = ArchJump(hdr->entry - 0xC0000000, bootdev, options);									// Try to jump!
		
		MemFree((UIntPtr)buffer);																			// ... Failed
		
		if (err == -1) {
			if (!rootdev) {																					// Failed (-1)
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowOkMessage("Couldn't jump to the kernel, please try again", Null);						// Show the error message
			goto l;
		} else if (err == -2) {
			if (!rootdev) {																					// Failed (-2)
				FsUmountFile("\\RootDevice");
				FsCloseFile(root);
			}
			
			MenuShowOkMessage("Your graphic card doesn't support any supported mode", Null);
			goto l;
		}
	} else {
		MenuShowOkMessage("Invalid boot type", opt->boot_type);												// Invalid boot type, show error message
		FsCloseFile(root);																					// Close the "root file"
		goto l;																								// And go back to the menu loop
	}
}

Void MenuInit(Void) {
	MenuOptions = ListNew(False);																			// Alloc the option list
	
	if (MenuOptions == Null) {
		ConWriteFormated("PANIC! Failed to init the menu\r\n");												// Failed
		ArchHalt();																							// Halt
	}
	
	ConfIterate(MenuIterator);																				// Init the option list
}
