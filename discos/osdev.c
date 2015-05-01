#include "output.h"
#include "multiboot.h"
#include "thread.h"
#include "descriptor_table.h"
#include "timer.h"
#include "fs.h"

void init(multiboot_info_t * mbt)
{
	terminal_initialize();
	//char a[20];
	//itoa(&file_system, a, 16);
	//prints(a);
	//prints('\n');
	
	init_tables();

	init_thread();
#if prem == 1
	init_timer();
#endif
	schedule();

}
