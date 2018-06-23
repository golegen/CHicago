/*
   File author is Ítalo Lima Marconato Matias
  
   Created on May 11 of 2018, at 13:18 BRT
   Last edited on May 12 of 2018, at 09:00 BRT
*/

ENTRY(KernelEntry)

SECTIONS
{
	. = 0x8000;
	
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
