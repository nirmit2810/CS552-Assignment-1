#ifndef __H_SCHEDULER__
#define __H_SCHEDULER__
#include "thread.h"


void add_to_queue(TCB * tcb);   //Add Thread to the queue 

void remove_from_queue(TCB * tcb);  //Remove Thread from the queue 

void priority_queue();           //Schedule queue based on static priorities
TCB * pick_next_in_queue();     //Pick the next available thread in the queue
#endif
