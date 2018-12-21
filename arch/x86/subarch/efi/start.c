// File author is Ítalo Lima Marconato Matias
//
// Created on December 19 of 2018, at 22:08 BRT
// Last edited on December 20 of 2018, at 18:18 BRT

#include <efi.h>
#include <efilib.h>

#include <chicago/types.h>

extern Void LoaderMain(Void);
extern EFI_HANDLE LibImageHandle;

EFI_STATUS LoaderEntry(EFI_HANDLE handle, EFI_SYSTEM_TABLE *st) {
	(Void)handle;									// Avoid the unused parameter warning
	LibImageHandle = handle;						// Save the boot image handle
	ST = st;										// Save the system table
	BS = ST->BootServices;							// And the boot services table
	
	BS->SetWatchdogTimer(0, 0, 0, NULL);			// Disable the watchdog timer
	LoaderMain();									// Go to LoaderMain!
	
	return EFI_LOAD_ERROR;							// LoaderMain should never return!
}
