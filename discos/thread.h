#ifndef __H_THREAD__
#define __H_THREAD__
#include "stdint.h"
#include "global_t.h"

#define NUM_THREADS 1
#define ESP = 0x0

#define prem 0                                       // prem 1 enables the preemptive version
#define dyn  0	                            //  dyn 1 enables static scheduling policy

typedef struct file_descriptor_st {
	uint32_t number;
	uint32_t index_node_number;
	uint32_t offset;
	uint32_t size;
  bool assigned ;
} file_descriptor;

#define MAX_FILE_DESCRIPTOR_NUM 1024

typedef struct thread_control_block_st {
	uint32_t stack_pointer;
	uint32_t thread_id;
	uint32_t * entry;
	uint32_t priority[2];
	bool assigned;
	file_descriptor fd[MAX_FILE_DESCRIPTOR_NUM];
} TCB;					//Define the TCB 

int thread_create( void * stack, void * function, int priority); //Creates the thread

int get_free_pcb();                         //Find the available control block

void schedule();                          // Schedule the thread

void yield();                            

void exit_thread();

void init_thread(void); 

extern TCB * current_pcb;

void add_to_table(file_descriptor fd); 

int delete_from_table(int fd);

int check_if_fd_exists(int fd);

file_descriptor * file_descriptor_entry(int fd);


#endif
