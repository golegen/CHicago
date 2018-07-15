// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:35 BRT
// Last edited on July 15 of 2018, at 20:30 BRT

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/list.h>
#include <chicago/string.h>

PList FsDeviceList = Null;
PChar FsBootDevice = Null;

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

Boolean FsRemoveDevice(PChar name) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return False;															// No...
	}
	
	PDevice dev = FsGetDevice(name);											// Try to get the device
	UIntPtr idx = 0;
	Boolean found = False;
	
	if (dev == Null) {															// Failed?
		return False;															// Yes, so this device doesn't exists
	}
	
	for (; !found && idx < FsDeviceList->length; idx++) {
		if (ListGet(FsDeviceList, idx) == dev) {								// Found?
			found = True;														// Yes!
		}
	}
	
	if (!found) {																// Found?
		return False;															// No (What? But we found it earlier)
	}
	
	if (ListRemove(FsDeviceList, idx) == Null) {								// Try to remove it
		return False;															// Failed...
	}
	
	if (FsGetDevice(name) != Null) {											// We really removed it?
		return False;															// No (for some reason)
	}
	
	MemFree((UIntPtr)(dev->name));												// Free the name
	MemFree((UIntPtr)dev);														// And the dev struct
	
	return True;																// AND RETURN TRUE!
}

PDevice FsGetDevice(PChar name) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return Null;															// No...
	}
	
	ListForeach(FsDeviceList, i) {												// Let's do an for in each (foreach) list entry
		PChar dname = ((PDevice)(i->data))->name;								// Save the entry name
		
		if (StrGetLength(dname) != StrGetLength(name)) {						// Same length?
			continue;															// No, so we don't even need to compare this entry
		} else if (StrCompare(dname, name)) {									// dname == name?
			return (PDevice)(i->data);											// YES!
		}
	}
	
	return Null;
}

Void FsSetBootDevice(PChar name) {
	if (FsGetDevice(name) != Null) {
		FsBootDevice = name;
	}
}

PChar FsGetBootDevice(Void) {
	return FsBootDevice;
}

Void FsDbgListDevices(Void) {
	if (FsDeviceList == Null) {
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
