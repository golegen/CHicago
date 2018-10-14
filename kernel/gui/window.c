// File author is Ítalo Lima Marconato Matias
//
// Created on October 14 of 2018, at 18:27 BRT
// Last edited on October 14 of 2018, at 18:28 BRT

#include <chicago/gui/window.h>
#include <chicago/mm.h>

static Boolean GuiGetWidget(PGuiWindow window, PGuiWidget widget, PUIntPtr out) {
	if (window == Null || widget == Null || ((UIntPtr)window) >= MM_USER_END || ((UIntPtr)widget) >= MM_USER_END) {			// Valid pointer?
		return False;																										// No >:(
	}
	
	UIntPtr idx = 0;
	
	ListForeach(&window->widgets, i) {																						// Let's search!
		if (i->data == widget) {																							// Found?
			if (out != Null) {																								// Yes! We should save the index?
				*out = idx;																									// Yes
			}
			
			return True;
		} else {
			idx++;																											// No, let's go to the next
		}
	}
	
	return False;																											// End of the list, we haven't found...
}

PGuiWindow GuiCreateWindow(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h) {
	PGuiWindow window = (PGuiWindow)MmAllocUserMemory(sizeof(GuiWindow));													// Let's try to allocate space for the window struct
	
	if (window != Null) {																									// Failed?
		window->widgets.head = Null;																						// No, so let's fill the informations!
		window->widgets.tail = Null;
		window->widgets.length = 0;
		window->widgets.free = False;
		window->widgets.user = True;
		window->tsurface.x = x;
		window->tsurface.y = y;
		window->tsurface.width = w + 10;
		window->tsurface.height = h + 25;
		window->usurface.x = x + 5;
		window->usurface.y = y + 20;
		window->usurface.width = w;
		window->usurface.height = h;
	}
	
	return window;
}

Void GuiFreeWindow(PGuiWindow window) {
	if (window == Null || ((UIntPtr)window) >= MM_USER_END) {																// Valid pointer?
		return;																												// No >:(
	}
	
	while (window->widgets.length > 0) {																					// First, free all the widgets
		PGuiWidget widget = (PGuiWidget)ListRemove(&window->widgets, window->widgets.length - 1);							// "Pop" the last widget in the list
		
		if (widget->free != Null) {
			widget->free(widget);																							// Free it
		}
		
		MmFreeUserMemory((UIntPtr)widget);																					// Free the widget struct itself
	}
	
	MmFreeUserMemory((UIntPtr)window);																						// Free the window struct itself
}

Void GuiAddWidget(PGuiWindow window, PGuiWidget widget) {
	if (window == Null || widget == Null || ((UIntPtr)window) >= MM_USER_END || ((UIntPtr)widget) >= MM_USER_END) {			// Valid pointer?
		return;																												// No >:(
	}
	
	if (GuiGetWidget(window, widget, Null)) {																				// Duplicate?
		return;																												// Yes >:(
	}
	
	ListAdd(&window->widgets, widget);																						// Let's try to add!
}

Void GuiRemoveWidget(PGuiWindow window, PGuiWidget widget) {
	if (window == Null || widget == Null || ((UIntPtr)window) >= MM_USER_END || ((UIntPtr)widget) >= MM_USER_END) {			// Valid pointer?
		return;																												// No >:(
	}
	
	UIntPtr idx;
	
	if (!GuiGetWidget(window, widget, &idx)) {																				// Let's try to get the widget index in the window
		return;																												// Failed...
	}
	
	ListRemove(&window->widgets, idx);																						// And remove it
}
