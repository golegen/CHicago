// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:35 BRT
// Last edited on January 21 of 2019, at 12:06 BRT

#include <chicago/alloc.h>
#include <chicago/console.h>
#include <chicago/device.h>
#include <chicago/list.h>
#include <chicago/string.h>
#include <chicago/subarch.h>

PList FsDeviceList = Null;
PChar FsBootDevice = Null;

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (dev->read != Null) {													// We can call the device's function?
		return dev->read(dev, off, len, buf);									// Yes!
	} else {
		return False;															// Nope, so return False
	}
}

Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8)) {
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
	
	if (!ListAdd(FsDeviceList, dev)) {											// Try to add to the list
		MemFree((UIntPtr)dev);													// Failed, so let's free the dev struct
		return False;															// And return False
	}
	
	return True;
}

Boolean FsAddHardDisk(PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8)) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return False;															// No...
	}
	
	static UIntPtr count = 0;
	UIntPtr nlen = StrFormat(Null, "HardDisk%d", count++);						// Get the length of the name
	PChar name = (PChar)MemAllocate(nlen);										// Alloc space for the name
	
	if (name == Null) {
		return False;															// Failed
	}
	
	StrFormat(name, "HardDisk%d", count - 1);									// Format the name string!
	
	if (!FsAddDevice(name, priv, read)) {											// Try to add it!
		MemFree((UIntPtr)name);													// Failed, free the name and return
		return False;
	}
	
	return True;
}

Boolean FsAddCdRom(PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8)) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return False;															// No...
	}
	
	static UIntPtr count = 0;
	UIntPtr nlen = StrFormat(Null, "CdRom%d", count++);							// Get the length of the name
	PChar name = (PChar)MemAllocate(nlen);										// Alloc space for the name
	
	if (name == Null) {
		return False;															// Failed
	}
	
	StrFormat(name, "CdRom%d", count - 1);										// Format the name string!
	
	if (!FsAddDevice(name, priv, read)) {										// Try to add it!
		MemFree((UIntPtr)name);													// Failed, free the name and return
		return False;
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

PChar FsGetDeviceByPriv(PVoid priv) {
	if (priv == Null) {															// Valid priv?
		return Null;															// Nope
	}
	
	ListForeach(FsDeviceList, i) {												// Let's search for the device with the requested priv
		if (((PDevice)(i->data))->priv == priv) {								// Found?
			return ((PDevice)(i->data))->name;									// Yes!
		}
	}
	
	return Null;
}

PDevice FsGetDeviceByID(UIntPtr id) {
	if (FsDeviceList == Null) {
		return Null;
	} else if (id >= FsDeviceList->length) {
		return Null;
	}
	
	return (PDevice)ListGet(FsDeviceList, id);
}

UIntPtr FsGetDeviceID(PChar name) {
	if (FsDeviceList == Null) {
		return 0;
	}
	
	UIntPtr idx = 0;
	
	ListForeach(FsDeviceList, i) {
		PChar dname = ((PDevice)(i->data))->name;
		
		if (StrGetLength(dname) != StrGetLength(name)) {
			idx++;
			continue;
		} else if (StrCompare(dname, name)) {
			return idx;
		} else {
			idx++;
		}
	}
	
	return (UIntPtr)-1;
}

Void FsSetBootDevice(PChar name) {
	if (FsGetDevice(name) != Null) {												// Device with this name exists?
		FsBootDevice = name;														// Yes! So set it as boot device (just like the user asked)
	}
}

PChar FsGetBootDevice(Void) {
	return FsBootDevice;
}

Void FsInitDeviceList(Void) {
	FsDeviceList = ListNew(True);													// Try to init the device list
	
	if (FsDeviceList == Null) {														// Failed?
		ConWriteFormated("PANIC! Couldn't init the device list\r\n");				// Yes, so halt
		ArchHalt();																	// Halt
	}
}
