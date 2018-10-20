// File author is Ítalo Lima Marconato Matias
//
// Created on October 14 of 2018, at 18:29 BRT
// Last edited on October 19 of 2018, at 21:53 BRT

#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/gui.h>
#include <chicago/mm.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/version.h>

List GuiWindowList;
Lock GuiRefreshLock = False;
Volatile Boolean GuiShouldRefresh = True;

static Void GuiRenderProcess(Void) {
	while (1) {																															// GUI render process main thread!
		if (GuiShouldRefresh) {																											// Refresh the screen?
			PsLock(&GuiRefreshLock);																									// Yes, lock
			DispFillRectangle(0, 0, DispGetWidth(), DispGetHeight(), 0xFF3A6EA5);														// Draw the background
			DispWriteFormated(DispGetWidth() - 194, DispGetHeight() - 32, 0xFF3A6EA5, 0xFFFFFFFF, "CHicago Operating System");			// Let's show the build number on the desktop
			DispWriteFormated(DispGetWidth() - CHICAGO_VSTR_LEN * 8 - 2, DispGetHeight() - 16, 0xFF3A6EA5, 0xFFFFFFFF, CHICAGO_VSTR);
			
			ListForeach(&GuiWindowList, i) {																							// Let's draw all the windows
				PGuiWindowInt wint = (PGuiWindowInt)i->data;																			// Get the priv data
				PGuiWindow window = wint->window;																						// This is the window
				UIntPtr oldd = MmGetCurrentDirectory();																					// Switch to the window's page directory
				
				if (wint->dir != oldd) {
					MmSwitchDirectory(wint->dir);
				}
				
				PGuiSurface tsurface = &window->tsurface;																				// Surface with the title bar
				PGuiSurface usurface = &window->usurface;																				// Surface without the title bar
				
				GuiFillRectangle(tsurface, 0, 0, 5, 20, 0xFF353535);																	// Let's draw the borders! First let's draw the diagonal resize borders
				GuiFillRectangle(tsurface, tsurface->width - 5, 0, 5, 20, 0xFF353535);
				GuiFillRectangle(tsurface, 0, tsurface->height - 5, 5, 5, 0xFF353535);
				GuiFillRectangle(tsurface, tsurface->width - 5, tsurface->height - 5, 5, 5, 0xFF353535);
				
				GuiFillRectangle(tsurface, 5, 0, usurface->width, 20, 0xFF353535);														// Now the other borders
				GuiFillRectangle(tsurface, 0, 20, 5, usurface->height, 0xFF353535);
				GuiFillRectangle(tsurface, 5, tsurface->height - 5, usurface->width, 5, 0xFF353535);
				GuiFillRectangle(tsurface, tsurface->width - 5, 20, 5, usurface->height, 0xFF353535);
				
				if (window->title != Null) {																							// We have an title?
					for (UIntPtr i = 0; i < StrGetLength(window->title); i++) {															// Yes!
						if ((i * 8) >= (tsurface->width - 40)) {																		// Too large?
							break;																										// Yes, stop
						} else {
							GuiWriteCharacter(tsurface, 5 + (i * 8), 3, 0xFFF5F5F5, 1, window->title[i]);								// No, keep on going!
						}
					}
				}
				
				GuiFillRectangle(tsurface, tsurface->width - 15, 5, 10, 10, 0xFFB22222);												// Close button
				GuiFillRectangle(tsurface, tsurface->width - 29, 5, 10, 10, 0xFFCCCC00);												// Maximize button
				GuiFillRectangle(tsurface, tsurface->width - 43, 5, 10, 10, 0xFF228B22);												// Minimize button
				
				GuiFillRectangle(usurface, 0, 0, usurface->width, usurface->height, 0xFFA9A9A9);										// The background of the window
				
				ListForeach(&window->widgets, i) {																						// Now let's draw the widgets!
					PGuiWidget widget = (PGuiWidget)i->data;
					
					if (widget->draw != Null) {																							// Draw function?
						widget->surface.x += usurface->x;
						widget->surface.y += usurface->y;
						
						widget->draw(widget);
						
						widget->surface.x -= usurface->x;
						widget->surface.y -= usurface->y;
					}
				}
				
				if (wint->dir != oldd) {																								// Need to switch back to the kernel directory?
					MmSwitchDirectory(oldd);																							// Yes
				}
			}
			
			DispRefresh();
			
			GuiShouldRefresh = False;
			
			PsUnlock(&GuiRefreshLock);
		}
	}
}

static Boolean GuiGetWindow(PGuiWindow window, UIntPtr dir, PUIntPtr out) {
	if (window == Null || ((UIntPtr)window) >= MM_USER_END) {																			// Valid pointer?
		return False;																													// No >:(
	}
	
	UIntPtr idx = 0;
	
	ListForeach(&GuiWindowList, i) {																									// Same as the above, but in the normal window list
		PGuiWindowInt wint = (PGuiWindowInt)i->data;
		
		if (wint->dir == dir && wint->window == window) {
			if (out != Null) {
				*out = idx;
			}
			
			return True;
		} else {
			idx++;
		}
	}
	
	return False;																														// Not found...
}

Void GuiAddWindow(PGuiWindow window) {
	if (window == Null || ((UIntPtr)window) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	PsLock(&GuiRefreshLock);																											// Lock
	PsLockTaskSwitch(old);																												// Switch to the kernel directory
	
	if (GuiGetWindow(window, oldd, Null)) {																								// Already in the list?
		PsUnlockTaskSwitch(old);																										// Yup
		PsUnlock(&GuiRefreshLock);
		return;
	}
	
	PGuiWindowInt wint = (PGuiWindowInt)MmAllocUserMemory(sizeof(GuiWindowInt));														// Alloc the internal struct
	
	if (wint != Null) {																													// Failed?
		wint->window = window;																											// No, so we can fill the info!
		wint->dir = oldd;
		
		if (ListAdd(&GuiWindowList, wint)) {																							// Try to add!		
			GuiShouldRefresh = True;																									// Ok!
		}
	}
	
	PsUnlockTaskSwitch(old);																											// Exit the kernel directory
	PsUnlock(&GuiRefreshLock);																											// Unlock
}

Void GuiRemoveWindow(PGuiWindow window) {
	if (window == Null || ((UIntPtr)window) >= MM_USER_END) {																			// Valid pointer?
		return;																															// No >:(
	}
	
	PsLock(&GuiRefreshLock);																											// Lock
	PsLockTaskSwitch(old);																												// Switch to the kernel directory
	
	UIntPtr idx = 0;
	
	if (GuiGetWindow(window, oldd, &idx)) {																								// Let's try to find this window!
		PGuiWindowInt wint = ListRemove(&GuiWindowList, idx);																			// Found! Let's try to remove it
		
		if (wint != Null) {																												// Ok?
			MmFreeUserMemory((UIntPtr)wint);																							// Yes, free the memory and set the should refresh flag
			GuiShouldRefresh = True;
		}
	}
	
	PsUnlockTaskSwitch(old);																											// Exit the kernel directory
	PsUnlock(&GuiRefreshLock);																											// Unlock
}

Void GuiRefresh(Void) {
	PsLock(&GuiRefreshLock);																											// Lock
	GuiShouldRefresh = True;																											// Set the refresh flag
	PsUnlock(&GuiRefreshLock);																											// Unlock
}

Void GuiInit(Void) {
	GuiWindowList.head = Null;																											// Init the window list
	GuiWindowList.tail = Null;
	GuiWindowList.length = 0;
	GuiWindowList.free = False;
	GuiWindowList.user = True;
	
	PProcess proc = PsCreateProcessInt("GuiRender", (UIntPtr)GuiRenderProcess, MmKernelDirectory);										// Create the gui render process
	
	if (proc == Null) {
		DbgWriteFormated("PANIC! Couldn't init the GUI\r\n");																			// Failed...
		while (1) ;
	}
	
	PsAddProcess(proc);																													// Add it
}
