#include "output.h"
#include "multiboot.h"
#include "thread.h"
#include "gdt.h"

void init(multiboot_info_t * mbt)
{
	init_gdt();
	
	terminal_initialize();

	init_thread();
	schedule();

}
