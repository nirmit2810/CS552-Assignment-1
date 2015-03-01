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
}
