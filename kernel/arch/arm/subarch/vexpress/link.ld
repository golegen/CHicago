/*
   File author is Ítalo Lima Marconato Matias
  
   Created on August 11 of 2018, at 14:29 BRT
   Last edited on August 11 of 2018, at 20:11 BRT
*/

ENTRY(KernelEntry)

SECTIONS
{
	. = 0x80010000;
	
	KernelStart = .;
	
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
	
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
	
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
	
	__bss_start = .;
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss .bss.*)
	}
	__bss_end = .;
	
	KernelEnd = .;
}
