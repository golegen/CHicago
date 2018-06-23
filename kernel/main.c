// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on June 18 of 2018, at 13:43 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/debug.h>

Void KernelMain(Void) {
	ArchInit();									// Start by initalizating the architecture-dependent bits of the kernel
	
	while (1) ;
}
