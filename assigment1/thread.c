#include "output.h"
#include "thread.h"
#include "scheduler.h"

static uint32_t stack1[1024] __attribute__ ((aligned (0x1000)));
static uint32_t stack2[1024] __attribute__ ((aligned (0x1000)));
static uint32_t stack3[1024] __attribute__ ((aligned (0x1000)));
static uint32_t stack4[1024] __attribute__ ((aligned (0x1000)));

static TCB threads[NUM_THREADS];

static TCB * current_pcb;
//priority
int p[4]={0,2,2,3};

static void thread1() {
	int x = 0;
#if prem == 0
	prints("1\n");
	yield();
	prints("Thread 1 's back\n");
#else
	while(1){
		for(int i = 0; i < 494967; i++);
		prints("1");
		x++;
		if(x==1000){
			prints("\nThread 1 is completed.\n");
			break;
		}
	}
#endif
}

static void thread2() {
	int y = 0;
#if prem == 0
	prints("2\n");
	yield();
	prints("Thread 2 's back\n");
#else
	while(1){
		for(int i = 0; i < 494967; i++);
		prints("2");
		y++;
		if(y==600){
	    prints("\nThread 2 is completed.\n");
			break;
		}
	}

#endif
}

static void thread3() {
	int z = 0;
#if prem == 0
	prints("3\n");
	yield();
	prints("Thread 3 's back\n");
#else
	while(1){
		for(int i = 0; i < 494967; i++);
		prints("3");
		z++;
		if(z==830){
			prints("\nThread 3 is completed.\n");
			break;
		}
	}

#endif
}
static void thread4() {
	int z = 0;
#if prem == 0
	prints("4\n");
	yield();
	prints("Thread 4 's back\n");
#else
	while(1){
		for(int i = 0; i < 494967; i++);
		prints("4");
		z++;
		if(z==530){
			prints("\nThread 4 is completed.\n");
			break;
		}
	}

#endif
}
int thread_create( void * stack, void * function, int priority) {
	//Input end of thread
	int pcb_id = -1;
	pcb_id = get_free_pcb();
	if( pcb_id == -1) {
		prints("No avaiable PCB\n");
		asm volatile("hlt\n\t");
	}

	//Setting exit point
	*((uint32_t *)stack) = (uint32_t) exit_thread;
	// Shifting stack pointer
	stack = (void *)((uint32_t *)stack - 1);

	TCB * current_PCB = &threads[pcb_id];
	current_PCB->thread_id = pcb_id;
	current_PCB->assigned = TRUE;
	current_PCB->entry = function;
	current_PCB->priority[0] = priority;
	current_PCB->priority[1] = 0;
	// The first 22 * 2 bytes for register states
	current_PCB->stack_pointer = (uint32_t) ((uint16_t *)stack - 22);

	*((uint32_t *) stack - 0) = (uint32_t) current_PCB->entry; // ENTRY POINT
#if prem == 0
	*((uint32_t *) stack - 1) = 0; // Interrupt
#else
	*((uint32_t *) stack - 1) = 1 << 9; // Interrupt
#endif
	*((uint32_t *) stack - 2) = 0; // EAX
	*((uint32_t *) stack - 3) = 0; // ECX
	*((uint32_t *) stack - 4) = (uint32_t)((uint32_t*) stack - 2); // EBX
	*((uint32_t *) stack - 5) = (uint32_t)((uint32_t*) stack - 2); // ESP
	*((uint32_t *) stack - 6) = 0; // ESI
	*((uint32_t *) stack - 7) = 0; // EDI
	*((uint32_t *) stack - 8) = 0x10; // DS
	*((uint32_t *) stack - 9) = 0x10; // ES
	*((uint32_t *) stack - 10) = 0x10; // FS
	*((uint32_t *) stack - 11) = 0x10; // GS
     *((uint32_t *) stack - 12)= ( uint32_t) current_PCB->priority[0];
     *((uint32_t *) stack - 12)= ( uint32_t) current_PCB->priority[1];
     
     
     
	add_to_queue(current_PCB);
}

void init_thread(void){
	thread_create(&stack1[1023], thread1, p[0]);
	thread_create(&stack2[1023], thread2, p[1]);
	thread_create(&stack3[1023], thread3, p[2]);
	thread_create(&stack4[1023], thread4, p[3]);
}

void yield(void) {
	schedule();
}

int get_free_pcb() {
	for(int i = 0; i < NUM_THREADS; i ++ )
		if(!threads[i].assigned) return i;
	return -1;
}

void schedule(){
	TCB * from_pcb = NULL;
	TCB * to_pcb = NULL;

	to_pcb = pick_next_in_queue();
	if(to_pcb == current_pcb) return;
	if(!to_pcb){
		prints("All PCB gone\n");
		asm volatile("cli \n\t");
		asm volatile("hlt \n\t");
	}
	from_pcb = current_pcb;
	current_pcb = to_pcb;
	asm volatile("call switch_thread\n\t" :: "S" (from_pcb), "D" (to_pcb));
}

void exit_thread(){
	current_pcb->assigned = FALSE;
	remove_from_queue(current_pcb);
	schedule();
}
