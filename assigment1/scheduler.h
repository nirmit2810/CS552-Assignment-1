#ifndef __H_SCHEDULER__
#define __H_SCHEDULER__
#include "thread.h"

void add_to_queue(TCB * tcb);

void remove_from_queue(TCB * tcb);

TCB * pick_next_in_queue();
#endif
