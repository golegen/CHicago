// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:38 BRT
// Last edited on July 15 of 2018, at 01:33 BRT

#ifndef __CHICAGO_DEVICE_H__
#define __CHICAGO_DEVICE_H__

#include <chicago/types.h>

typedef struct DeviceStruct {
	PChar name;
	PVoid priv;
	Boolean (*read)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*write)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
} Device, *PDevice;

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsWriteDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*write)(PDevice, UIntPtr, UIntPtr, PUInt8));
PDevice FsGetDevice(PChar name);
Void FsDbgListDevices(Void);
Void FsInitDevices(Void);

#endif		// __CHICAGO_DEVICE_H__
