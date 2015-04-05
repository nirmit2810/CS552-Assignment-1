OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS
{
	. = 1M + SIZEOF_HEADERS;

	. = ALIGN(4K);
	.text : { *(.text)}

	. = ALIGN(4K);
	.rodata : { *(.rodata) }

	. = ALIGN(4K);
	.data : { *(.data)}

	. = ALIGN(4K);

	.bss : { 
		*(.bss COMMON)
		}
}
