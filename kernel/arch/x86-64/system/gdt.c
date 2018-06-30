// File author is Ítalo Lima Marconato Matias
//
// Created on May 12 of 2018, at 21:15 BRT
// Last edited on June 29 of 2018, at 18:01 BRT

#include <chicago/arch/gdt.h>

extern Void GDTLoad(UIntPtr base, UInt16 limit);

UInt8 GDTEntries[5][8];

Void GDTSetGate(UInt8 num, UInt32 base, UInt32 limit, UInt8 type, UInt8 gran) {
	GDTEntries[num][0] = 0xFF;												// Encode the limit (and granularity)
	GDTEntries[num][1] = 0xFF;
	GDTEntries[num][6] = (UInt8)((gran << 4) | ((limit >> 16) & 0x0F));
	
	GDTEntries[num][2] = base & 0xFF;										// Encode the base
	GDTEntries[num][3] = (base >> 8) & 0xFF;
	GDTEntries[num][4] = (base >> 16) & 0xFF;
	GDTEntries[num][7] = (base >> 24) & 0xFF;
	
	GDTEntries[num][5] = type;												// Encode the type
}

Void GDTInit(Void) {
	GDTSetGate(0, 0, 0, 0, 0);												// Null
	GDTSetGate(1, 0, 0xFFFFFFFF, 0x98, 0x02);								// Kernel code
	GDTSetGate(2, 0, 0xFFFFFFFF, 0x92, 0x00);								// Kernel data
	GDTSetGate(3, 0, 0xFFFFFFFF, 0xF8, 0x02);								// User code
	GDTSetGate(4, 0, 0xFFFFFFFF, 0xF2, 0x00);								// User data
	GDTLoad((UIntPtr)GDTEntries, 39);										// Load the new GDT
}
