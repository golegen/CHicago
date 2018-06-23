// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:22 BRT
// Last edited on May 31 of 2018, at 17:05 BRT

#include <chicago/arch/gdt-int.h>

UInt8 GDTEntries[5][8];

Void GDTSetGate(UInt8 num, UInt32 base, UInt32 limit, UInt8 type)
{
	if (limit > 0xFFFF) {										// Adjust granularity
		limit = limit >> 12;
		GDTEntries[num][6] = 0xC0;
	} else {
		GDTEntries[num][6] = 0x40;
	}
	
	GDTEntries[num][0] = limit & 0xFF;							// Encode the limit
	GDTEntries[num][1] = (limit >> 8) & 0xFF;
	GDTEntries[num][6] |= (limit >> 16) & 0x0F;
	
	GDTEntries[num][2] = base & 0xFF;							// Encode the base
	GDTEntries[num][3] = (base >> 8) & 0xFF;
	GDTEntries[num][4] = (base >> 16) & 0xFF;
	GDTEntries[num][7] = (base >> 24) & 0xFF;
	
	GDTEntries[num][5] = type;									// Encode the type
}

Void GDTInit(Void)
{
	GDTSetGate(0, 0, 0, 0);										// Null entry
	GDTSetGate(1, 0, 0xFFFFFFFF, 0x9A);							// Code entry
	GDTSetGate(2, 0, 0xFFFFFFFF, 0x92);							// Data entry
	GDTSetGate(3, 0, 0xFFFFFFFF, 0xFA);							// User mode code entry
	GDTSetGate(4, 0, 0xFFFFFFFF, 0xF2);							// User mode data entry
	GDTLoad((UInt32)GDTEntries, 39);							// Load new GDT
}
