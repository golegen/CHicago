// File author is Ítalo Lima Marconato Matias
//
// Created on October 19 of 2018, at 21:40 BRT
// Last edited on October 19 of 2018, at 21:40 BRT

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/process.h>
#include <chicago/stack.h>

Stack RawMouseDeviceStack;
Lock RawMouseDeviceStackLock = False;

static Boolean RawMouseDeviceReadInt(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;
	RawMouseDeviceRead(len, buf);															// Redirect to RawMouseDeviceRead
	return True;
}

Void RawMouseDeviceWrite(Int8 offx, Int8 offy, UInt8 buttons) {
	PsLock(&RawMouseDeviceStackLock);														// Lock
	StackPush(&RawMouseDeviceStack, (PVoid)(offx | (offy << 8) | (buttons << 16)));			// Push to the stack
	PsUnlock(&RawMouseDeviceStackLock);														// Unlock!
}

Void RawMouseDeviceRead(UIntPtr len, PUInt8 buf) {
	if (len == 0) {
		return;
	}
	
	while (RawMouseDeviceStack.length < len) {												// Let's fill the stack with what we need
		PsSwitchTask(Null);
	}
	
	PsLock(&RawMouseDeviceStackLock);														// Lock
	
	for (UIntPtr i = len; i > 0; i--) {														// Fill the buffer!
		UIntPtr cmd = (UIntPtr)StackPop(&RawMouseDeviceStack);
		
		buf[i + ((i - 1) * 3) - 1] = (UInt8)(cmd & 0xFF);
		buf[i + ((i - 1) * 3)] = (UInt8)((cmd >> 8) & 0xFF);
		buf[i + ((i - 1) * 3) + 1] = (UInt8)((cmd >> 16) & 0xFF);
	}
	
	PsUnlock(&RawMouseDeviceStackLock);														// Unlock!
}

Void RawMouseDeviceInit(Void) {
	RawMouseDeviceStack.head = Null;
	RawMouseDeviceStack.tail = Null;
	RawMouseDeviceStack.length = 0;
	RawMouseDeviceStack.free = False;
	RawMouseDeviceStack.user = False;
	
	if (!FsAddDevice("RawMouse", Null, RawMouseDeviceReadInt, Null, Null)) {
		DbgWriteFormated("PANIC! Failed to add the RawMouse device\r\n");
		while (1) ;
	}
}
