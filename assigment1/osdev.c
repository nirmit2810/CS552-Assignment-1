#include "output.h"
#include "multiboot.h"
#include "thread.h"
#include "descriptor_table.h"
#include "timer.h"

void init(multiboot_info_t * mbt)
{
	
	terminal_initialize();
	init_tables();

	init_thread();
#if prem == 1
	init_timer();
#endif
	schedule();

}
