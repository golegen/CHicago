// File author is Ítalo Lima Marconato Matias
//
// Created on December 20 of 2018, at 18:15 BRT
// Last edited on April 19 of 2019, at 17:45 BRT

#define StrDuplicate Dummy

#include <efi.h>
#include <efilib.h>
#include <lib.h>

#undef StrDuplicate

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/device.h>
#include <chicago/list.h>
#include <chicago/port.h>
#include <chicago/string.h>

static EFI_DEVICE_PATH_PROTOCOL *BootDevicePath;
static PVoid BootDevicePriv;

static EFI_DEVICE_PATH_PROTOCOL *GetDP(EFI_HANDLE handle, PUIntPtr count) {
	EFI_DEVICE_PATH_PROTOCOL *dp;
	Boolean status = BS->HandleProtocol(handle, &gEfiDevicePathProtocolGuid, (VOID*)&dp);																		// Let's try to open the device path (using BS->HandleProtocol)
	
	if (EFI_ERROR(status)) {
		return NULL;																																			// Failed...
	} else if (IsDevicePathEnd(dp)) {
		return NULL;																																			// ... i don't think this should happen... just return NULL
	}
	
	for (EFI_DEVICE_PATH_PROTOCOL *next = NextDevicePathNode(dp); !IsDevicePathEnd(next); dp = next, next = NextDevicePathNode(next), (*count)++) ;				// Get the last dp entry
	
	return dp;																																					// And return
}

static EFI_DEVICE_PATH_PROTOCOL *GetDPn(EFI_HANDLE handle, IntPtr i) {
	EFI_DEVICE_PATH_PROTOCOL *dp;
	Boolean status = BS->HandleProtocol(handle, &gEfiDevicePathProtocolGuid, (VOID*)&dp);																		// Let's try to open the device path (using BS->HandleProtocol)
	
	if (EFI_ERROR(status)) {
		return NULL;																																			// Failed...
	} else if (IsDevicePathEnd(dp)) {
		return NULL;																																			// ... i don't think this should happen... just return NULL
	}
	
	for (EFI_DEVICE_PATH_PROTOCOL *next = NextDevicePathNode(dp); !IsDevicePathEnd(next) && i; dp = next, next = NextDevicePathNode(next), i--) ;				// Get the entry i
	
	return dp;																																					// And return
}

static Boolean DeviceRead(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	EFI_BLOCK_IO_PROTOCOL *priv = (EFI_BLOCK_IO_PROTOCOL*)dev->priv;
	UIntPtr bsize = priv->Media->BlockSize;
	
	UIntPtr cur = off / bsize;
	UIntPtr end = (off + len) / bsize;
	
	if ((off % bsize) != 0) {																																	// "Align" the start
		PUInt8 buff = (PUInt8)MemAllocate(bsize);																												// Alloc memory for reading the disk
		
		if (buff == Null) {
			return False;																																		// Failed...
		} else if (EFI_ERROR(priv->ReadBlocks(priv, priv->Media->MediaId, cur, bsize, buff))) {																	// Read this block
			MemFree((UIntPtr)buff);																																// Failed...
			return False;
		}
		
		StrCopyMemory(buf, buff + (off % bsize), bsize - (off % bsize));																						// Copy it into the user buffer
		MemFree((UIntPtr)buff);
		cur++;
	}
	
	if (((off + len) % bsize) != 0) {																															// "Align" the end
		PUInt8 buff = (PUInt8)MemAllocate(bsize);																												// Alloc memory for reading the disk
		
		if (buff == Null) {
			return False;																																		// Failed...
		} else if (EFI_ERROR(priv->ReadBlocks(priv, priv->Media->MediaId, end, bsize, buff))) {																	// Read this block
			MemFree((UIntPtr)buff);																																// Failed...
			return False;
		}
		
		StrCopyMemory(buf + len - ((off + len) % bsize), buff, (off + len) % bsize);																			// Copy it into the user buffer
		MemFree((UIntPtr)buff);
		
		if (end != 0) {																																			// Only decrease the end if it isn't 0
			end--;
		}
	}
	
	if (cur < end) {																																			// Let's read!
		if (EFI_ERROR(priv->ReadBlocks(priv, priv->Media->MediaId, cur, (end - cur) * bsize, buf + (off % bsize)))) {
			return False;																																		// Failed...
		}
	}
	
	return True;
}

static Int32 ArchGetMemoryMap(Void) {
	EFI_MEMORY_DESCRIPTOR *descs = Null;
	UINTN dsize = 0, size = 0, key = 0;
	UInt32 dver = 0;
	EFI_STATUS ret = BS->GetMemoryMap(&size, descs, &key, &dsize, &dver);																						// First, let's try to get the buffer size
	
	if (ret == EFI_BUFFER_TOO_SMALL) {
		descs = (EFI_MEMORY_DESCRIPTOR*)MemZAllocate(size);																										// :) Alloc space for the mem descs
		
		if (descs == Null) {
			return -1;																																			// Failed...
		}
		
		ret = BS->GetMemoryMap(&size, descs, &key, &dsize, &dver);																								// And call BS->GetMemoryMap to get the mem descs
	}
	
	if (EFI_ERROR(ret) || (descs == NULL)) {
		return -1;																																				// Failed...
	}
	
	for (UINTN i = 0; i < size; i += dsize) {																													// Let's write all the mmap entries!
		EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR*)(((PUInt8)descs) + i);
		PUInt64 mmap64 = (PUInt64)(0x3000 + ((i / dsize) * 24));
		
		*mmap64++ = desc->PhysicalStart;																														// base
		*mmap64++ = desc->NumberOfPages * 0x1000;																												// length
		
		PUInt32 mmap32 = (PUInt32)mmap64;
		
		switch (desc->Type) {																																	// And the type!
		case EfiReservedMemoryType:
		case EfiUnusableMemory:
		case EfiACPIMemoryNVS:
		case EfiMemoryMappedIO:
		case EfiMemoryMappedIOPortSpace:
		case EfiPalCode:
			*mmap32++ = 0x02;																																	// (Reserved)
		default:
			*mmap32++ = 0x01;																																	// (Free)
		}
	}
	
	MemFree((UIntPtr)descs);																																	// Free the temp mem descs
	
	return size / dsize;
}

static Boolean ArchSetVideoMode(UInt16 width, UInt16 height, PUInt32 data) {
	EFI_HANDLE *hbuf;
	UINTN hcount, msize, mnum;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *mode;
	
	if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &hcount, &hbuf))) {													// Let's try to find the gop handle buffer
		return False;																																			// Failed :(
	} else if (EFI_ERROR(BS->HandleProtocol(hbuf[0], &gEfiGraphicsOutputProtocolGuid, (VOID**)&gop))) {															// Now let's try to get the gop itself
		return False;																																			// Failed :(
	}
	
	for (mnum = 0; !EFI_ERROR(gop->QueryMode(gop, mnum, &msize, &mode)); mnum++) {																				// Now let's try to find the request resolution!
		if ((mode->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) && (mode->HorizontalResolution == width) && (mode->VerticalResolution == height)) {	// Found?
			break;																																				// Yes!
		}
	}
	
	if ((mode->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) || (mode->HorizontalResolution != width) || (mode->VerticalResolution != height)) {		// Found?
		return False;																																			// No...
	} else if (EFI_ERROR(gop->SetMode(gop, mnum))) {																											// Set!
		return False;																																			// Failed :(
	}
	
	if (data != Null) {																																			// Set out data?
		data[0] = width;																																		// Yes!
		data[1] = height;
		data[2] = 32;
		data[3] = (UInt32)gop->Mode->FrameBufferBase;
	}
	
	return True;
}

IntPtr ArchJump(UIntPtr dest, PChar bootdev, UIntPtr options) {
	if (dest == 0 || bootdev == Null) {																															// Valid dest and boot device?
		return -1;																																				// Nope
	}
	
	Int32 mmapc = ArchGetMemoryMap();																															// Get the memory map
	
	if (mmapc == -1) {
		return -1;																																				// Failed
	}
	
	PUInt32 data = (PUInt32)0x2FF0;
	
	if (ArchSetVideoMode(1440, 900, data)) {																													// Set the video mode to 1440x900
		goto c;																																					// Ok
	} else if (ArchSetVideoMode(1360, 768, data)) {																												// Set the video mode to 1360x768
		goto c;																																					// Ok
	} else if (ArchSetVideoMode(1280, 800, data)) {																												// Set the video mode to 1280x800
		goto c;																																					// Ok!
	} else if (ArchSetVideoMode(1280, 720, data)) {																												// Set the video mode to 1280x720
		goto c;																																					// Ok!
	} else if (ArchSetVideoMode(800, 600, data)) {																												// Set the video mode to 800x600
		goto c;																																					// Ok!
	} else {																																					// Couldn't set any supported video mode...
		return -2;
	}

c:	;
	UINTN msize, mkey, dsize;
	
	BS->GetMemoryMap(&msize, NULL, &mkey, &dsize, NULL);																										// Call GetMemoryMap, we need the mkey
	
	if (!EFI_ERROR(BS->ExitBootServices(LibImageHandle, mkey))) {																								// ExitBootServices!
		ArchJumpInt(dest, bootdev, 0x3000, mmapc, (UIntPtr)data, options);																						// And jump!
	}
	
	return -1;																																					// ... Returned?
}

static Void ArchAddDevice(EFI_HANDLE *handles, UINTN i, UInt8 type, UInt8 subtype, Int cdrom) {
	UIntPtr n = 0;
	EFI_DEVICE_PATH_PROTOCOL *dp = GetDP(handles[i], &n);
	
	if (dp == NULL) {
		return;																																					// ...
	} else if ((DevicePathType(dp) != type) || (DevicePathSubType(dp) != subtype)) {																			// Valid?
		for (IntPtr j = n - 1; j >= 0; j--) {																													// Nope, maybe this is a partition inside of the boot cdrom/boot hard disk!
			EFI_DEVICE_PATH_PROTOCOL *dpn = GetDPn(handles[i], j);
			
			if ((DevicePathType(dpn) == type) && (DevicePathSubType(dpn) == subtype)) {																			// Same type and subtype that we are expecting?
				EFI_BLOCK_IO_PROTOCOL *priv;

				if (EFI_ERROR(BS->OpenProtocol(handles[i], &gEfiBlockIoProtocolGuid, (VOID**)&priv, LibImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL))) {	// Yes, let's try to open the protocol using the handle
					return;																																		// Failed :(
				}
				
				if (priv == BootDevicePriv) {																													// Same boot device priv?
					BootDevicePath = dpn;																														// Yes!
					return;
				}
			}
		}
		
		return;																																					// Nope
	}
	
	EFI_BLOCK_IO_PROTOCOL *priv;
	
	if (EFI_ERROR(BS->OpenProtocol(handles[i], &gEfiBlockIoProtocolGuid, (VOID**)&priv, LibImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL))) {				// Open the protocol using the handle
		return;																																					// Failed :(
	} else if (priv->Media->BlockSize != 512 && cdrom) {																										// CDROM?
		FsAddCdRom((PVoid)priv, DeviceRead);																													// Yes, add it
	} else if (priv->Media->BlockSize == 512) {
		FsAddHardDisk((PVoid)priv, DeviceRead);																													// Hard disk
	}
	
	if ((DevicePathNodeLength(BootDevicePath) == DevicePathNodeLength(dp)) && !StrCompareMemory(BootDevicePath, dp, DevicePathNodeLength(dp) + 4)) {			// Update the boot device priv?
		BootDevicePriv = priv;																																	// Yes!
	}
}

Void ArchInit(Void) {
	FsInitDeviceList();																																			// Init the device list
	
	EFI_LOADED_IMAGE_PROTOCOL *img;
	PVoid priv;
	
	if (EFI_ERROR(BS->OpenProtocol(LibImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&img, LibImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL))) {		// Open the loaded image protocol using the image handle
		ConWriteFormated("PANIC! Couldn't determine the boot device\r\n");																						// Failed
		ArchHalt();
	} else if (EFI_ERROR(BS->OpenProtocol(img->DeviceHandle, &gEfiBlockIoProtocolGuid, &priv, LibImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL))) {			// Open the block io protocol using the device handle
		ConWriteFormated("PANIC! Couldn't determine the boot device\r\n");																						// Failed
		ArchHalt();
	} else {
		BootDevicePriv = priv;
	}
	
	UINTN size = 0;																																				// Let's try to add "all" the devices!
	EFI_HANDLE *handles = NULL;
	EFI_STATUS ret = BS->LocateHandle(ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &size, handles);																// First, let's try to get the buffer size
	
	if (ret == EFI_BUFFER_TOO_SMALL) {
		handles = (EFI_HANDLE*)MemZAllocate(size);																												// :) Alloc the device list
		
		if (handles == Null) {
			ConWriteFormated("PANIC! Out of memory\r\n");																										// Failed...
			ArchHalt();
		}
		
		ret = BS->LocateHandle(ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &size, handles);																		// And get call BS->LocateHandle to copy the device list to our list
	}
	
	if (EFI_ERROR(ret) || (handles == NULL)) {
		ConWriteFormated("PANIC! Failed to get the block io device list\r\n");																					// Failed...
		ArchHalt();
	}
	
	for (UINTN i = 0; i < size / sizeof(EFI_HANDLE); i++) {																										// First, let's get all the IDE devices
		ArchAddDevice(handles, i, MESSAGING_DEVICE_PATH, MSG_ATAPI_DP, 0);
	}
	
	for (UINTN i = 0; i < size / sizeof(EFI_HANDLE); i++) {
		ArchAddDevice(handles, i, MESSAGING_DEVICE_PATH, MSG_ATAPI_DP, 1);
	}
	
	for (UINTN i = 0; i < size / sizeof(EFI_HANDLE); i++) {																										// Now, let's get all the SATA devices
		ArchAddDevice(handles, i, MESSAGING_DEVICE_PATH, MSG_SATA_DP, 0);
	}
	
	for (UINTN i = 0; i < size / sizeof(EFI_HANDLE); i++) {
		ArchAddDevice(handles, i, MESSAGING_DEVICE_PATH, MSG_SATA_DP, 1);
	}
	
	MemFree((UIntPtr)handles);																																	// Free the device list!
	FsSetBootDevice(FsGetDeviceByPriv(BootDevicePriv));																											// Set the boot device
	
	if (FsGetBootDevice() == Null) {																															// Unsupported boot device?
		ConWriteFormated("PANIC! Unsupported boot device\r\n");																									// Yes :(
		ArchHalt();
	}
}
