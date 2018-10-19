// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 21:08 BRT
// Last edited on October 13 of 2018, at 13:33 BRT

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/process.h>
#include <chicago/stack.h>

Stack RawKeyboardDeviceStack;
Lock RawKeyboardDeviceStackLock = False;
Volatile Boolean RawKeyboardDeviceNeedKey = False;

static Boolean RawKeyboardDeviceReadInt(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;
	RawKeyboardDeviceRead(len, buf);														// Redirect to RawKeyboardDeviceRead
	return True;
}

Void RawKeyboardDeviceWrite(UInt8 data) {
	PsLock(&RawKeyboardDeviceStackLock);													// Lock
	StackPush(&RawKeyboardDeviceStack, (PVoid)data);										// Push to the stack
	PsUnlock(&RawKeyboardDeviceStackLock);													// Unlock!
}

Void RawKeyboardDeviceRead(UIntPtr len, PUInt8 buf) {
	if (len == 0) {
		return;
	}
	
	while (RawKeyboardDeviceStack.length < len) {											// Let's fill the stack with the chars that we need
		RawKeyboardDeviceNeedKey = True;
		PsSwitchTask(Null);
	}
	
	PsLock(&RawKeyboardDeviceStackLock);													// Lock
	
	for (UIntPtr i = len; i > 0; i--) {														// Fill the buffer!
		buf[i - 1] = (UInt8)StackPop(&RawKeyboardDeviceStack);
	}
	
	PsUnlock(&RawKeyboardDeviceStackLock);													// Unlock!
}

Boolean RawKeyboardDeviceWaitingKey(Void) {
	return RawKeyboardDeviceNeedKey;
}

Void RawKeyboardDeviceInit(Void) {
	RawKeyboardDeviceStack.last = Null;														// Init the keyboard stack
	RawKeyboardDeviceStack.length = 0;
	RawKeyboardDeviceStack.user = False;
	
	if (!FsAddDevice("RawKeyboard", Null, RawKeyboardDeviceReadInt, Null, Null)) {			// Try to add the keyboard device
		DbgWriteFormated("PANIC! Failed to add the RawKeyboard device\r\n");					// Failed...
		while (1) ;
	}
}
