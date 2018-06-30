// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on June 30 of 2018, at 11:56 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/port.h>
#include <chicago/arch/registers.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/heap.h>
#include <chicago/panic.h>

PChar PanicStrings[4] = {
	"MM_READWRITE_TO_NONPRESENT_AREA",
	"MM_WRITE_TO_READONLY_AREA",
	"KERNEL_UNEXPECTED_ERROR",
	"KERNEL_INIT_FAILED"
};

Void ArchPanicWriteHex(UInt32 val) {
	if (val < 0x10) {
		DispWriteFormated("0x0000000%x", val);
	} else if (val < 0x100) {
		DispWriteFormated("0x000000%x", val);
	} else if (val < 0x1000) {
		DispWriteFormated("0x00000%x", val);
	} else if (val < 0x10000) {
		DispWriteFormated("0x0000%x", val);
	} else if (val < 0x100000) {
		DispWriteFormated("0x000%x", val);
	} else if (val < 0x1000000) {
		DispWriteFormated("0x00%x", val);
	} else if (val < 0x10000000) {
		DispWriteFormated("0x0%x", val);
	} else {
		DispWriteFormated("0x%x", val);
	}
}

Void ArchPanic(UInt32 err, PVoid data) {
	PRegisters regs = (PRegisters)data;
	UInt32 cr2 = 0;
	
	Asm Volatile("mov %%cr2, %0" : "=r"(cr2));
	
	DispSetCursor(0, 0, True);
	DispSetColor(DISP_COLOR_RED, DISP_COLOR_WHITE);
	DispClearScreen();
	
	DispWriteFormated("Sorry, CHicago got an fatal error and halted\r\n");
	DispWriteFormated("You need to restart your computer manually\r\n\r\n");
	DispWriteFormated("Error Code: %s\r\n\r\n", PanicStrings[err]);
	
	DispWriteFormated("| EAX: "); ArchPanicWriteHex(regs->eax); DispWriteFormated(" | ");
	DispWriteFormated("EBX: "); ArchPanicWriteHex(regs->ebx); DispWriteFormated(" | ");
	DispWriteFormated("ECX:    "); ArchPanicWriteHex(regs->ecx); DispWriteFormated(" | ");
	DispWriteFormated("EDX: "); ArchPanicWriteHex(regs->edx); DispWriteFormated(" |\r\n");
	
	DispWriteFormated("| ESI: "); ArchPanicWriteHex(regs->esi); DispWriteFormated(" | ");
	DispWriteFormated("EDI: "); ArchPanicWriteHex(regs->edi); DispWriteFormated(" | ");
	DispWriteFormated("ESP:    "); ArchPanicWriteHex(regs->esp); DispWriteFormated(" | ");
	DispWriteFormated("EBP: "); ArchPanicWriteHex(regs->ebp); DispWriteFormated(" |\r\n");
	
	DispWriteFormated("| EIP: "); ArchPanicWriteHex(regs->eip); DispWriteFormated(" | ");
	DispWriteFormated("CR2: "); ArchPanicWriteHex(cr2); DispWriteFormated(" | ");
	DispWriteFormated("EFLAGS: "); ArchPanicWriteHex(regs->eflags); DispWriteFormated(" | ");
	DispWriteFormated("                |\r\n");
	
	DispWriteFormated("| CS:  "); ArchPanicWriteHex((UInt8)regs->cs); DispWriteFormated(" | ");
	DispWriteFormated("DS:  "); ArchPanicWriteHex((UInt8)regs->ds); DispWriteFormated(" | ");
	DispWriteFormated("ES:     "); ArchPanicWriteHex((UInt8)regs->es); DispWriteFormated(" | ");
	DispWriteFormated("FS:  "); ArchPanicWriteHex((UInt8)regs->fs); DispWriteFormated(" |\r\n");
	
	DispWriteFormated("| GS:  "); ArchPanicWriteHex((UInt8)regs->gs); DispWriteFormated(" | ");
	DispWriteFormated("SS:  "); ArchPanicWriteHex((UInt8)regs->ss); DispWriteFormated(" | ");
	DispWriteFormated("                   | ");
	DispWriteFormated("                |\r\n");
	
	while (1) {
		Asm Volatile("pause");
	}
}

Void ArchInit(Void) {
	SerialInit(COM1_PORT);																								// Init debugging (using COM1 port)
	DebugWriteFormated("COM1 initialized\r\n");
	
	if (!DispPreInit()) {																								// Alloc the display buffer and map it to the temp page directory
		DebugWriteFormated("PANIC! Couldn't init the display\r\n");														// Failed...
		while (1) ;
	} else {
		DebugWriteFormated("Disp initialized\r\n");
	}
	
	if (MultibootHeaderMagic != 0x2BADB002) {
		Panic(PANIC_KERNEL_INIT_FAILED);
	} else {
		MultibootHeaderPointer->boot_device += 0xC0000000;																// Fix the multiboot pointer structure
		MultibootHeaderPointer->cmd_line += 0xC0000000;
		MultibootHeaderPointer->mods_address += 0xC0000000;
		MultibootHeaderPointer->address += 0xC0000000;
		MultibootHeaderPointer->mmap_address += 0xC0000000;
		MultibootHeaderPointer->drives_address += 0xC0000000;
		MultibootHeaderPointer->config_table += 0xC0000000;
		MultibootHeaderPointer->boot_loader_name += 0xC0000000;
		MultibootHeaderPointer->apm_table += 0xC0000000;
		MultibootHeaderPointer->vbe_control_info += 0xC0000000;
		MultibootHeaderPointer->vbe_mode_info += 0xC0000000;
	}
	
	GDTInit();																											// Init the GDT
	DebugWriteFormated("GDT initialized\r\n");
	
	IDTInit();																											// Init the IDT
	DebugWriteFormated("IDT initialized\r\n");
	
	VMMPreInit();																										// Alloc the VMM structs
	PMMInit();																											// Init the PMM
	DebugWriteFormated("PMM initialized\r\n");
	
	VMMInit();																											// Init the VMM
	HeapInit(KernelRealEnd, 0xFFC00000);																				// Init the kernel heap (start after all the internal kernel structs and end at the start of the temp addresses)
	
	if (!DispInit()) {																									// Init the display buffer again, this time in the kernel page directory
		DebugWriteFormated("PANIC! Couldn't init the display\r\n");														// Failed...
		while (1) ;
	} else {
		DebugWriteFormated("VMM initialized\r\n");
	}
	
	Panic(PANIC_KERNEL_INIT_FAILED);																					// Test the panic function!
}
