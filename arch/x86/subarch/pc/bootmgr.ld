/*
   File author is Ítalo Lima Marconato Matias
  
   Created on October 22 of 2018, at 15:33 BRT
   Last edited on December 19 of 2018, at 23:26 BRT
*/

OUTPUT_FORMAT("binary")

SECTIONS
{
	. = 0x7E00;

	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : { *(.bss COMMON) }

	/DISCARD/ :
	{
		*(.note.GNU-stack)
		*(.gnu_debuglink)
		*(.gnu.lto_*)
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
		
	LoaderEnd = .;
}
