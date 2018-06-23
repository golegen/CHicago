/*
   File author is Ítalo Lima Marconato Matias
  
   Created on May 11 of 2018, at 13:21 BRT
   Last edited on June 22 of 2018, at 13:49 BRT
*/

ENTRY(KernelEntry)

SECTIONS
{
	. = 0xC0100000;
	
	KernelStart = .;
	
	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
	
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
	
	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}
	
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss .bss.*)
	}
	
	KernelEnd = .;
	
	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}
