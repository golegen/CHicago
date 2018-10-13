// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 21:08 BRT
// Last edited on October 13 of 2018, at 14:22 BRT

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/process.h>
#include <chicago/stack.h>

Stack KeyboardDeviceStack;
Lock KeyboardDeviceStackLock = False;
Volatile Boolean KeyboardDeviceNeedKey = False;

static Boolean KeyboardDeviceReadInt(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;
	KeyboardDeviceRead(len, buf);															// Redirect to KeyboardDeviceRead
	return True;
}

Void KeyboardDeviceWrite(UInt8 data) {
	PsLock(&KeyboardDeviceStackLock);														// Lock
	StackPush(&KeyboardDeviceStack, (PVoid)data);											// Push to the stack
	PsUnlock(&KeyboardDeviceStackLock);														// Unlock!
}

Void KeyboardDeviceRead(UIntPtr len, PUInt8 buf) {
	if (len == 0) {
		return;
	}
	
	while (KeyboardDeviceStack.length < len) {												// Let's fill the stack with the chars that we need
		KeyboardDeviceNeedKey = True;
		PsSwitchTask(Null);
	}
	
	PsLock(&KeyboardDeviceStackLock);														// Lock
	
	for (UIntPtr i = len; i > 0; i--) {														// Fill the buffer!
		buf[i - 1] = (UInt8)StackPop(&KeyboardDeviceStack);
	}
	
	PsUnlock(&KeyboardDeviceStackLock);														// Unlock!
}

Boolean KeyboardDeviceWaitingKey(Void) {
	return KeyboardDeviceNeedKey;
}

Void KeyboardDeviceInit(Void) {
	KeyboardDeviceStack.last = Null;														// Init the keyboard stack
	KeyboardDeviceStack.length = 0;
	KeyboardDeviceStack.user = False;
	
	if (!FsAddDevice("Keyboard", Null, KeyboardDeviceReadInt, Null, Null)) {				// Try to add the keyboard device
		DbgWriteFormated("PANIC! Failed to add the Keyboard device\r\n");					// Failed...
		while (1) ;
	}
}
