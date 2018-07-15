// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:19 BRT
// Last edited on July 15 of 2018, at 13:57 BRT

#include <chicago/arch/gdt.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/multiboot.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/serial.h>

#include <chicago/debug.h>

MultibootHeader64 MultibootHeaderPointer64;

UIntPtr MmGetPhys(UIntPtr virt) { (Void)virt; return 0; }
Boolean MmMap(UIntPtr virt, UIntPtr phys, UInt32 flags) { (Void)virt; (Void)phys; (Void)flags; return False; }
Boolean MmUnmap(UIntPtr virt) { (Void)virt; return 0; }

Void ArchInit(Void) {
	SerialInit(COM1_PORT);																								// Init debugging (using COM1 port)
	DbgWriteFormated("[x86-64] COM1 initialized\r\n");
	
	if (MultibootHeaderMagic != 0x2BADB002) {
		DbgWriteFormated("PANIC! We need GRUB (or any other multiboot-compilant bootloader)\r\n");
		while (1) ;
	} else {
		MultibootHeaderPointer64.flags = MultibootHeaderPointer32->flags;												// Fix the multiboot pointer structure
		MultibootHeaderPointer64.mem_lower = MultibootHeaderPointer32->mem_lower;
		MultibootHeaderPointer64.mem_upper = MultibootHeaderPointer32->mem_upper;
		MultibootHeaderPointer64.boot_device = MultibootHeaderPointer32->boot_device;
		MultibootHeaderPointer64.cmd_line = MultibootHeaderPointer32->cmd_line + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.mods_count = MultibootHeaderPointer32->mods_count;
		MultibootHeaderPointer64.mods_address = MultibootHeaderPointer32->mods_address + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.num = MultibootHeaderPointer32->num;
		MultibootHeaderPointer64.size = MultibootHeaderPointer32->size;
		MultibootHeaderPointer64.address = MultibootHeaderPointer32->address + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.shndx = MultibootHeaderPointer32->shndx;
		MultibootHeaderPointer64.mmap_length = MultibootHeaderPointer32->mmap_length;
		MultibootHeaderPointer64.mmap_address = MultibootHeaderPointer32->mmap_address + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.drives_length = MultibootHeaderPointer32->drives_length;
		MultibootHeaderPointer64.drives_address = MultibootHeaderPointer32->drives_address + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.config_table = MultibootHeaderPointer32->config_table + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.boot_loader_name = MultibootHeaderPointer32->boot_loader_name + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.apm_table = MultibootHeaderPointer32->apm_table + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.vbe_control_info = MultibootHeaderPointer32->vbe_control_info + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.vbe_mode_info = MultibootHeaderPointer32->vbe_mode_info + 0xFFFFFFFF80000000;
		MultibootHeaderPointer64.vbe_mode = MultibootHeaderPointer32->vbe_mode;
		MultibootHeaderPointer64.vbe_interface_segment = MultibootHeaderPointer32->vbe_interface_segment;
		MultibootHeaderPointer64.vbe_interface_offset = MultibootHeaderPointer32->vbe_interface_offset;
		MultibootHeaderPointer64.vbe_interface_length = MultibootHeaderPointer32->vbe_interface_length;
		MultibootHeaderPointer = &MultibootHeaderPointer64;
	}
	
	GDTInit();																											// Init the GDT
	DbgWriteFormated("[x86-64] GDT initialized\r\n");
	
	IDTInit();																											// Init the IDT
	DbgWriteFormated("[x86-64] IDT initialized\r\n");
	
	PMMInit();																											// Init the PMM
	DbgWriteFormated("[x86-64] PMM initialized\r\n");
}
