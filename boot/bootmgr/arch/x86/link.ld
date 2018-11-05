/*
   File author is Ítalo Lima Marconato Matias
  
   Created on October 22 of 2018, at 15:33 BRT
   Last edited on November 05 of 2018, at 15:55 BRT
*/

OUTPUT_FORMAT("binary")

SECTIONS
{
	. = 0x7E00;

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
		
	LoaderEnd = .;
}
