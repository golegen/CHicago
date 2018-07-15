// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:35 BRT
// Last edited on July 15 of 2018, at 01:43 BRT

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/list.h>

PList FsDeviceList = Null;

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (dev->read != Null) {
		return dev->read(dev, off, len, buf);
	} else {
		return False;
	}
}

Boolean FsWriteDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (dev->write != Null) {
		return dev->write(dev, off, len, buf);
	} else {
		return False;
	}
}

Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*write)(PDevice, UIntPtr, UIntPtr, PUInt8)) {
	if (FsDeviceList == Null) {
		return False;
	}
	
	PDevice dev = (PDevice)MemAllocate(sizeof(Device));
	PChar nm = Null;
	UIntPtr nlen = 0;
	
	for (; name[nlen] != 0; nlen++) ;
	
	nm = (PChar)MemAllocate(nlen + 1);
	
	if ((dev == Null) || (nm == Null)) {
		if (dev != Null) {
			MemFree((UIntPtr)dev);
		}
		if (nm != Null) {
			MemFree((UIntPtr)name);
		}
		
		return False;
	}
	
	for (UIntPtr i = 0; i < nlen; i++) {
		nm[i] = name[i];
	}
	
	dev->name = nm;
	dev->priv = priv;
	dev->read = read;
	dev->write = write;
	
	if (!ListAdd(FsDeviceList, dev)) {
		return False;
	}
	
	return True;
}

PDevice FsGetDevice(PChar name) {
	if (FsDeviceList == Null) {
		return Null;
	}
	
	UIntPtr nl = 0;
	
	for (; name[nl] != 0; nl++) ;
	
	ListForeach(FsDeviceList, i) {
		PChar dname = ((PDevice)(i->data))->name;
		Boolean equal = True;
		Boolean end = False;
		UIntPtr dnl = 0;
		
		for (; dname[dnl] != 0; dnl++) ;
		
		if (dnl !=  nl) {
			continue;
		}
		
		for (UInt32 j = 0; equal && !end; j++) {
			if (dname[j] != name[j]) {
				equal = False;
			} else if (dname[j] == '\0') {
				end = True;
			}
			
			if (equal) {
				return (PDevice)(i->data);
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
