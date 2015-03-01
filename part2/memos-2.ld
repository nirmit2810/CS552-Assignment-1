OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
PHDRS
{
	physical PT_LOAD FILEHDR PHDRS ;
}
SECTIONS
{
	. = 1M + SIZEOF_HEADERS;
	. = ALIGN(4K);
	.text : { *(.text)} :physical

	. = ALIGN(4K);
	.rodata : { *(.rodata)} :physical

	. = ALIGN(4K);
	.data : { *(.data)} :physical

	. = ALIGN(4K);
	.bss : { 
		*(COMMON)
		*(.data)
		*(.bootstrap_stack)
		} :physical
}
