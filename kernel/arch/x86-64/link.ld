/*
   File author is Ítalo Lima Marconato Matias
  
   Created on July 11 of 2018, at 00:46 BRT
   Last edited on July 12 of 2018, at 12:35 BRT
*/

ENTRY(KernelEntry32)

SECTIONS
{
	. = 0xFFFFFFFF80200000;
	
	KernelStart = .;
	
	.text ALIGN(4K) : AT(ADDR(.text) - 0xFFFFFFFF80000000)
	{
		*(.multiboot)
		*(.text)
	}
	
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
	{
		*(.rodata)
	}
	
	.data ALIGN(4K) : AT(ADDR(.data) - 0xFFFFFFFF80000000)
	{
		*(.data)
	}
	
	KernelBssStart = .;
	
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
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
