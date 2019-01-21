// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:38 BRT
// Last edited on January 21 of 2019, at 11:52 BRT

#ifndef __CHICAGO_DEVICE_H__
#define __CHICAGO_DEVICE_H__

#include <chicago/types.h>

typedef struct DeviceStruct {
	PChar name;
	PVoid priv;
	Boolean (*read)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
} Device, *PDevice;

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8));
Boolean FsAddHardDisk(PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8));
Boolean FsAddCdRom(PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8));
Boolean FsRemoveDevice(PChar name);
PDevice FsGetDevice(PChar name);
PChar FsGetDeviceByPriv(PVoid priv);
PDevice FsGetDeviceByID(UIntPtr id);
UIntPtr FsGetDeviceID(PChar name);
Void FsSetBootDevice(PChar name);
PChar FsGetBootDevice(Void);
Void FsInitDeviceList(Void);
Void FsInitDevices(Void);

#endif		// __CHICAGO_DEVICE_H__
