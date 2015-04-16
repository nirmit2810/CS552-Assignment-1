#include "thread.h"
#include "output.h"

static TCB * run_queue[NUM_THREADS];
static TCB * prior_queue;
static int current_index = -1;
int check=0;
int j=0;
int prior[NUM_THREADS][2];

void add_to_queue(TCB * tcb){
	int i;
	for(i = 0; i < NUM_THREADS; i ++) {
		if(run_queue[i] == NULL){
			prior[j][0]= i;
			prior[j][1]= tcb->priority;
			run_queue[i] = tcb;
		    j++;
			return;
		}
	}
	prints("Error: Run queue already full");
}

void priority_queue( ){

	int i=0;
	while( run_queue[i] != NULL) {
		i++;
		if(i==NUM_THREADS){
			break;
		}
	}
	if(i>1){
	    for (int x = 0; x < i; ++x) {
			for (int n = x + 1; n < i; ++n){
				if (prior[x][1] > prior[n][1]){
					int a =  prior[x][1];
					prior[x][1] = prior[n][1];
					prior[n][1] = a;
					prior_queue= run_queue[prior[x][0]];
					run_queue[prior[x][0]]=run_queue[prior[n][0]];
					run_queue[prior[n][0]]=prior_queue;
                }
			}
		}
    }
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
	if(check==0){
	check=1;
	prints("Run queue emptied\n");
}
	return NULL;

}
