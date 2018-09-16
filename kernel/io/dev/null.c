// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 13:16 BRT
// Last edited on September 15 of 2018, at 17:36 BRT

#include <chicago/debug.h>
#include <chicago/device.h>

Boolean NullDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off; (Void)len; (Void)buf;									// Avoid compiler's unused parameter warning
	return True;																// Always return True
}

Void NullDeviceInit(Void) {
	if (!FsAddDevice("Null", Null, Null, NullDeviceWrite, Null)) {				// Let's add ourself
		DbgWriteFormated("PANIC! Failed to add the Null device\r\n");			// Failed...
		while (1) ;
	}
}
