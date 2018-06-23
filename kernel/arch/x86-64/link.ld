/*
   File author is Ítalo Lima Marconato Matias
  
   Created on May 11 of 2018, at 13:19 BRT
   Last edited on May 12 of 2018, at 09:02 BRT
*/

ENTRY(KernelEntry32)

SECTIONS
{
	. = 1M;
	
	KernelStart = .;
	
	.text :
	{
		*(.multiboot)
		*(.text)
	}
	
	.rodata :
	{
		*(.rodata)
	}
	
	.data :
	{
		*(.data)
	}
	
	.bss :
	{
		*(COMMON)
	}
}
