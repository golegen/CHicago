// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 14:29 BRT
// Last edited on October 25 of 2018, at 15:25 BRT

#include <chicago/alloc.h>
#include <chicago/config.h>
#include <chicago/console.h>
#include <chicago/keyboard.h>
#include <chicago/list.h>
#include <chicago/menu.h>
#include <chicago/string.h>
#include <chicago/version.h>

PList MenuOptions = Null;
PChar MenuDefault = Null;
UIntPtr MenuSelectedOption = 0;

static Boolean MenuCompareStrings(PChar a, PChar b) {
	if (a == Null && b == Null) {													// Both are Null?
		return True;																// Yes
	} else if (a == Null || b == Null) {											// One is Null and the other one isn't?
		return False;																// Yes
	} else if (StrGetLength(a) != StrGetLength(b)) {								// Same length?
		return False;																// Nope
	}
	
	return StrCompare(a, b);														// Compare!
}

static PMenuOption MenuGetOption(PChar name) {
	if (name == Null) {																// Sanity check
		return Null;
	}
	
	ListForeach(MenuOptions, i) {													// Let's search!
		PMenuOption opt = (PMenuOption)i->data;
		
		if (MenuCompareStrings(name, opt->name)) {									// Found?
			return opt;																// Yes!
		}
	}
	
	return Null;																	// Not found...
}

static UIntPtr MenuGetOptionIdx(PChar name) {
	if (name == Null) {																// Sanity check
		return 0;
	}
	
	UIntPtr idx = 0;
	
	ListForeach(MenuOptions, i) {													// Let's search!
		PMenuOption opt = (PMenuOption)i->data;
		
		if (MenuCompareStrings(name, opt->name)) {									// Found?
			return idx;																// Yes!
		} else {
			idx++;
		}
	}
	
	return 0;																		// Not found...
}

static Void MenuIterator(PConfField field) {
	if (MenuCompareStrings(field->name, "default")) {								// Is the set default entry command?
		MenuDefault = field->value;													// Yes
		return;
	}
	
	PMenuOption opt = MenuGetOption(field->name);									// Let's see if this option already exists
	Boolean add = True;
	
	if (opt == Null) {
		opt = (PMenuOption)MemAllocate(sizeof(MenuOption));							// Nope, it doesn't, so let's alloc some space
	} else {
		add = False;																// Yes, it exists!
	}
	
	if (opt == Null) {
		ConWriteFormated("PANIC! Couldn't init the menu\r\n");						// Failed to alloc...
		while (1) ;
	}
	
	opt->name = field->name;														// Set the name
	opt->device = field->value;														// Set the device
	
	if (field->attrs == Null) {
		ConWriteFormated("PANIC! Couldn't init the menu\r\n");						// No attributes...
		while (1) ;
	}
	
	opt->boot_type = (PChar)ListGet(field->attrs, field->attrs->length - 1);		// Set the boot type!
	
	if (add) {
		if (!ListAdd(MenuOptions, opt)) {											// Add to the menu option list!
			ConWriteFormated("PANIC! Couldn't init the menu\r\n");					// Failed...
			while (1) ;
		}
	}
}

Void MenuLoop(Void) {
	ConHideCursor();																// Hide the cursor
	
	MenuSelectedOption = MenuGetOptionIdx(MenuDefault);								// Select the default option
	
	while (1) {
		if (MenuSelectedOption >= MenuOptions->length) {							// Fix the current selected option
			MenuSelectedOption = MenuOptions->length - 1;
		}
		
		ConClearScreen();															// Clear the screen
		ConWriteFormated("CHicago Boot Manager\r\n%s\r\n\r\n", CHICAGO_VSTR);		// Print the banner
		
		UIntPtr idx = 0;
		
		ListForeach(MenuOptions, i) {												// Print all the options
			PMenuOption opt = (PMenuOption)i->data;
			
			if (idx == MenuSelectedOption) {										// Selected one?
				ConSetColor(CON_COLOR_INVERSE);										// Yes, print with other color
			}
			
			ConWriteFormated("    %s\r\n", opt->name);
			
			if (idx++ == MenuSelectedOption) {
				ConSetColor(CON_COLOR_NORMAL);										// Revert back to the normal color
			}
			
			if (i->next == Null) {
				ConWriteFormated("\r\n");
			}
		}
		
		ConWriteFormated("Select an option with the arrow keys\r\n");				// Print the footer
		ConWriteFormated("Press Enter to select the highlighted option\r\n");
		
		UInt8 key = KbdReadKey();													// Wait for keypress
		
		if (key == 0x1C) {															// Enter
			break;
		} else if (key == 0x48) {													// Key down
			MenuSelectedOption--;
		} else if (key == 0x50) {													// Key up
			MenuSelectedOption++;
		}
	}
}

Void MenuInit(Void) {
	MenuOptions = ListNew(False);													// Alloc the option list
	
	if (MenuOptions == Null) {
		ConWriteFormated("PANIC! Failed to init the menu\r\n");						// Failed
		while (1) ;
	}
	
	ConfIterate(MenuIterator);														// Init the option list
}
