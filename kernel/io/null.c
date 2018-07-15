// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 13:16 BRT
// Last edited on July 15 of 2018, at 13:24 BRT

#include <chicago/debug.h>
#include <chicago/device.h>

Boolean NullDeviceRead(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off; (Void)len; (Void)buf;									// Avoid compiler's unused parameter warning
	return False;																// Always return False
}

Boolean NullDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off; (Void)len; (Void)buf;									// Avoid compiler's unused parameter warning
	return True;																// Always return False
}

Void NullDeviceInit(Void) {
	if (!FsAddDevice("Null", Null, NullDeviceRead, NullDeviceWrite)) {			// Let's add ourself
		DbgWriteFormated("[Kernel] Failed to add Null device\r\n");				// Failed...
	}
}
