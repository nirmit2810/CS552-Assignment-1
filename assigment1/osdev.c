#include "output.h"
#include "multiboot.h"
#include "thread.h"

void init(multiboot_info_t * mbt)
{
	terminal_initialize();

	init_thread();
	schedule();

}
