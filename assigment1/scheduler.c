#include "thread.h"
#include "output.h"

static TCB * run_queue[NUM_THREADS];
static int current_index = -1;


void add_to_queue(TCB * tcb){
	int i;
	for(i = 0; i < NUM_THREADS; i ++) {
		if(run_queue[i] == NULL){
			run_queue[i] = tcb;
			return;
		}
	}
	prints("Error: Run queue already full");
}

void remove_from_queue(TCB * tcb){
	int i;
	for(i = 0; i < NUM_THREADS; i ++) {
		if( run_queue[i] == tcb){
			run_queue[i] = NULL;
			return;
		}
	}
	prints("Error: Can't find tcb to remove from runqueue");
}

TCB * pick_next_in_queue(){
	if(current_index == -1) {
		for(int i = 0; i < NUM_THREADS; i ++) {
			if(run_queue[i] != NULL){
				current_index = i;
				return run_queue[i];
			}
		}
	} else {
		for(int i = current_index + 1; i < NUM_THREADS; i ++) {
			if(run_queue[i] != NULL){
				current_index = i;
				return run_queue[i];
			}
		}
		for(int i = 0; i <= current_index; i++) {
			if(run_queue[i] != NULL){
				current_index = i;
				return run_queue[i];
			}
		}
	}
	prints("Run queue emptied");
	return NULL;
}
