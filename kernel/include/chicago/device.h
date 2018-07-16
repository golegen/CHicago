// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:38 BRT
// Last edited on July 16 of 2018, at 18:21 BRT

#ifndef __CHICAGO_DEVICE_H__
#define __CHICAGO_DEVICE_H__

#include <chicago/types.h>

typedef struct DeviceStruct {
	PChar name;
	PVoid priv;
	Boolean (*read)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*write)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
} Device, *PDevice;

Void NullDeviceInit(Void);
Void ZeroDeviceInit(Void);

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsWriteDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*write)(PDevice, UIntPtr, UIntPtr, PUInt8));
Boolean FsRemoveDevice(PChar name);
PDevice FsGetDevice(PChar name);
PDevice FsGetDeviceByID(UIntPtr id);
UIntPtr FsGetDeviceID(PChar name);
Void FsSetBootDevice(PChar name);
PChar FsGetBootDevice(Void);
Void FsDbgListDevices(Void);
Void FsInitDeviceList(Void);
Void FsInitDevices(Void);

#endif		// __CHICAGO_DEVICE_H__
