OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS
{
	. = 1M + SIZEOF_HEADERS;

	.text : { *(.text)}

	.rodata : { *(.rodata) }

	.data : { *(.data)}

	.bss : { 
		*(.bss COMMON)
		}
}
