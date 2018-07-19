// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 22:24 BRT
// Last edited on July 19 of 2018, at 02:28 BRT

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/display.h>
#include <chicago/string.h>

Boolean FrameBufferDeviceRead(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev;																					// Avoid compiler's unused parameter warning
	
	if ((off + len) >= (DispGetWidth() * DispGetHeight() * DispGetBytesPerPixel())) {			// Too high address?
		return False;																			// Yes!
	} else {
		StrCopyMemory(buf, (PVoid)(DispGetFrameBuffer() + off), len);							// No, so let's read from the real framebuffer!
		return True;
	}
}

Boolean FrameBufferDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev;																					// Avoid compiler's unused parameter warning
	
	if ((off + len) >= (DispGetWidth() * DispGetHeight() * DispGetBytesPerPixel())) {			// Too high address?
		return False;																			// Yes...
	} else {
		StrCopyMemory((PVoid)(DispGetFrameBuffer() + off), buf, len);							// No, so let's write to the real framebuffer!
		return True;
	}
}

Void FrameBufferDeviceInit(Void) {
	if (!FsAddDevice("FrameBuffer", Null, FrameBufferDeviceRead, FrameBufferDeviceWrite)) {		// Let's add ourself
		DbgWriteFormated("[Kernel] Failed to add FrameBuffer device\r\n");						// Failed
	}
}
