/*
   File author is Ítalo Lima Marconato Matias
  
   Created on October 22 of 2018, at 13:37 BRT
   Last edited on October 22 of 2018, at 13:37 BRT
*/

OUTPUT_FORMAT("binary")
ENTRY(BootEntry)

SECTIONS
{
	. = 0x7C00;

	.text :
	{
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
		*(.bss)
	}

	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}
