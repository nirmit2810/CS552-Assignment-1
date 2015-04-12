#ifndef __H_THREAD__
#define __H_THREAD__
#include "stdint.h"

#define NUM_THREADS 3
#define ESP = 0x0

#define prem 0

typedef enum {
  FALSE,
  TRUE
} bool;

typedef struct thread_control_block_st {
	uint32_t stack_pointer;
	uint32_t thread_id;
	uint32_t * entry;
	bool assigned;
} TCB;

int thread_create( void * stack, void * function);

int get_free_pcb();

void schedule();

void yield();

void exit_thread();

void init_thread(void);

#endif
