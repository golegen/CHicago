// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:35 BRT
// Last edited on July 15 of 2018, at 12:10 BRT

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/list.h>

PList FsDeviceList = Null;

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (dev->read != Null) {													// We can call the device's function?
		return dev->read(dev, off, len, buf);									// Yes!
	} else {
		return False;															// Nope, so return False
	}
}

Boolean FsWriteDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (dev->write != Null) {													// We can call the device's function?
		return dev->write(dev, off, len, buf);									// Yes!
	} else {
		return False;															// Nope, so return False
	}
}

Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*write)(PDevice, UIntPtr, UIntPtr, PUInt8)) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return False;															// No...
	}
	
	PDevice dev = (PDevice)MemAllocate(sizeof(Device));							// Allocate memory for the dev struct
	
	if (dev == Null) {															// Failed?
		return False;															// Yes...
	}
	
	dev->name = name;
	dev->priv = priv;
	dev->read = read;
	dev->write = write;
	
	if (!ListAdd(FsDeviceList, dev)) {											// Try to add to the list
		MemFree((UIntPtr)dev);													// Failed, so let's free the dev struct
		return False;															// And return False
	}
	
	return True;
}

PDevice FsGetDevice(PChar name) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return Null;															// No...
	}
	
	UIntPtr nl = 0;
	
	for (; name[nl] != 0; nl++) ;												// First let's get the name length
	
	ListForeach(FsDeviceList, i) {												// Now let's do an for in each (foreach) list entry
		PChar dname = ((PDevice)(i->data))->name;								// Save the entry name
		Boolean equal = True;
		Boolean end = False;
		UIntPtr dnl = 0;
		
		for (; dname[dnl] != 0; dnl++) ;										// Get the entry name length
		
		if (dnl !=  nl) {														// They are equal?
			continue;															// Nope!
		}
		
		for (UInt32 j = 0; equal && !end; j++) {								// Yes, so let's compare then
			if (dname[j] != name[j]) {											// The char at j in entry's name is equals to the char at j in the name?
				equal = False;													// Nope...
			} else if (dname[j] == '\0') {										// End of string?
				end = True;														// YES!
			}
			
			if (equal) {														// Equals?
				return (PDevice)(i->data);										// Yes, so return it
			}
		}
	}
	
	return Null;
}

Void FsDbgListDevices(Void) {
	if (!FsDeviceList) {
		DbgWriteFormated("[FsDbgListDevices] Device list isn't initialized!\r\n");
	} else if (FsDeviceList->length == 0) {
		DbgWriteFormated("[FsDbgListDevices] No devices avaliable.\r\n");
	} else {
		ListForeach(FsDeviceList, i) {
			DbgWriteFormated("[FsDbgListDevices] %s\r\n", ((PDevice)(i->data))->name);
		}
	}
}

Void FsInitDevices(Void) {
	FsDeviceList = ListNew(True);
}
