#ifndef __H_SCHEDULER__
#define __H_SCHEDULER__
#include "thread.h"


/*

	Nirmit:

	Here's what I need from you:

	1.
	If you look inside scheduler.c, you will find a very straight forwrad way 
	to implement first in first priority queue (hopefully i didn't make any mistake)
	I would want you to implmement a dynamic priorty scheduler for extra credit.
	Basically you first have to read scheduler.c to understand what I'm doing and what are the steps that
	are needed to do for adding stuff into queue/removing from queue.
	Then you can modify the TCB structure in thread.h to impose some constraint on priority
	e.g. you can add a deadline, priorty flag or something like that..
	And then you'd have to modify the algorithm so that it would work with the new priorty.
	Right now the way it is implmeented, it would alwyas run in a round-robin fashin

	2. 
	The senocd thing that I'd like you to do (you've possibly done it before)
	is to allow scrolling on the display
	essentailly, what I had inside mmeos-2.c  for the display output is all migrated inside 
	output.h/c (SEE the files it will make sense)
	I would want you to make it so that it would continue to dispaly text on the screen even if the texts
	go beyond
	Also, I would like to see the cursor at the end of the texts, rather than some random pace like right now.

	Let me know if you have any further question
	-Tim

*/

void add_to_queue(TCB * tcb);

void remove_from_queue(TCB * tcb);

void priority_queue();
TCB * pick_next_in_queue();
#endif
