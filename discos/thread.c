#include "output.h"
#include "thread.h"
#include "scheduler.h"
#include "fs_func.h"
#include "fs_debug.h"

//TO BE DELETED
#include "randisk_test.c"

static uint32_t stack1[1024];
static uint32_t stack2[1024];
//static uint32_t stack3[1024];
//static uint32_t stack4[1024];

static TCB threads[NUM_THREADS];

TCB * current_pcb;

//priority
int p[4]={0,2,2,3};

//static void fs_test1() {
	//for(int i = 0; i < 8 + 10; i++) {
	//	for(int j = 0; j < 16; j++) {
	//		char a [7] = "/usr1/";
	//		a[4] = i % 10 + 48;
	//		rd_mkdir(a);
	//	}
	//}
  //rd_mkdir("/usr");
	//print_fs_in(&(file_system.ins[0]));
	//rd_creat("/usr/bin");
	//print_fs_in(&(file_system.ins[1]));
  //rd_creat("/usr/abc");
	//print_fs_in(&(file_system.ins[1]));
	//print_fs_in(&(file_system.ins[2]));
	//print_fs_in(&(file_system.ins[3]));
	//println("");
	//rd_unlink("/usr/abc");
	//print_fs_in(&(file_system.ins[0]));
	//print_fs_in(&(file_system.ins[1]));
	//print_fs_in(&(file_system.ins[2]));
	//print_fs_in(&(file_system.ins[3]));
	//rd_unlink("/usr/bin");
	//println("");
	//print_fs_in(&(file_system.ins[0]));
	//print_fs_in(&(file_system.ins[1]));
	//print_fs_in(&(file_system.ins[2]));
	//print_fs_in(&(file_system.ins[3]));
	//rd_unlink("/usr");
	//println("");
	//print_fs_in(&(file_system.ins[0]));
	//print_fs_in(&(file_system.ins[1]));
	//print_fs_in(&(file_system.ins[2]));
	//print_fs_in(&(file_system.ins[3]));
  //  rd_open("/was");
   // rd_open("/usr");
  //rd_mkdir("/direc");
  //rd_open("/direc");
   // rd_open("/was");
  //  rd_close(1);
#define STRLEN(s) (sizeof(s)/sizeof(s[0]))
	char a[] = \
	"\
0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345639489348\
";

	//const int num = 8 + 64 + 10;
	//int ee;
	//for(int i = 0; i < num; i++) {
	//	ee = rd_write(usr, a, STRLEN(a));
	//}
  //ee = rd_write(usr, a, STRLEN(a));
	//printnln(ee);

	//rd_lseek(usr, 0);

  //char b[STRLEN(a)];

  //int c;

	//for(int i = 0; i < num; i++) {
	//	c = rd_read(usr, b ,STRLEN(a));
	//	printnln(c);
	//}
	//println(b);
	//println(b);

  //c = rd_read(was, b ,4);
	//println(b);
   
  // rd_open("/wssf");
  // rd_open("/was");
  //rd_creat("/wssf");
  //rd_open("/wssf");   
  //rd_open("/usr");
	//rd_creat("/usr/cs552/123");
	//rd_mkdir("/usr/cs553");
	//rd_mkdir("/usr/cs554");
	//rd_mkdir("/usr/cs555");
	//rd_mkdir("/usr/cs556");
	//rd_mkdir("/usr/cs557");
	//rd_mkdir("/usr/cs557");
	//rd_mkdir("/usr/cs559");
	//rd_mkdir("/usr");
	//rd_mkdir("/usr/abc");
	//rd_mkdir("/usr/abc/edf");
	//rd_mkdir("/usr/abc/edf");
	//printnln(file_system.ins[0].locations[0]);
	//print_fs_in(&(file_system.ins[0]));
	//print_fs_in(&(file_system.ins[1]));
	//print_fs_in(&(file_system.ins[2]));
//}

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
	thread_create(&stack1[1023], fs_test1, p[0]);
	//thread_create(&stack2[1023], fs_test2, p[0]);
	//thread_create(&stack2[1023], thread2, p[1]);
	//thread_create(&stack3[1023], thread3, p[2]);
	//thread_create(&stack4[1023], thread4, p[3]);
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
