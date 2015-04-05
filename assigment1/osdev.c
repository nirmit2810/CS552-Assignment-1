#include "output.h"
#include "multiboot.h"
#include "thread.h"
#include "descriptor_table.h"

void init(multiboot_info_t * mbt)
{
	init_gdt();
	
	terminal_initialize();

	init_thread();
	schedule();

}
