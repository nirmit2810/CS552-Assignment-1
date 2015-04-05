#include "output.h"
#include "multiboot.h"
//#include "thread.h"
 
#define crBegin static int state=0; switch(state) { case 0:

#define crReturn(x) do { state=__LINE__; return x; \
                         case __LINE__:; } while (0)
#define crFinish }

#define MAX_THREADS 3
typedef enum {
  FALSE,
  TRUE
} booly;
 
static unsigned char memory[1024 * 1024]; //1 MB space for malloc
static size_t currentsz = 0;
void *malloc(size_t x)
{
    void *mem;
    if(sizeof memory - currentsz < x)
        return NULL;
    mem = &memory[currentsz];
    currentsz += x;
    return mem;
}

typedef struct runqueue {
  int (*task)();
  int tid; // thread ID
  struct runqueue *next;
  struct runqueue *prev;
} rq;
static rq head;
static booly done[MAX_THREADS];

int (*f[MAX_THREADS])(void);


int thread1 (void) {

  int i;
  static int j;

  crBegin;
  while (1) {
    for (i = 0; i < 10; i++) {

          terminal_writestring("<1>");
          for(int j=0;j<400000;j++);
        // usleep (100000);
    }
   
     terminal_writestring("\n");
    crReturn (1); // Let's cooperate and yield
     
    if (++j == 6)
      break;
  }
  done[0] = TRUE;
	terminal_writestring("Done <1>! \n");
  crFinish;

  return 1;
}


int thread2 (void) {

  int i;
  static int j;

  crBegin;
  while (1) {
    for (i = 0; i < 5; i++) {
          terminal_writestring("<2>");
          for(int j=0;j<400000;j++);
      //usleep (100000);
      //fflush (stdout);
    }
    prints("\n");
    crReturn (2); // Time to yield

    if (++j == 10)
      break;
  }
  done[1] = TRUE;
 
  terminal_writestring("Done <2>! \n");
  crFinish;

  return 2;
}
int thread3 (void) {

  int i;
  static int j;

  crBegin;
  while (1) {
    for (i = 0; i < 2; i++) {
     terminal_writestring("<3>");
     for(int j=0;j<400000;j++);
 //     usleep (100000); 	
      //fflush (stdout);
    }
    prints("\n");
    crReturn (3); // Time to yield

    if (++j == 15)
      break;
  }
  done[2] = TRUE;

terminal_writestring("Done <3>! \n");
  crFinish;

  return 3;
}

void schedule (void) {

  rq *current; // Current thread in runqueue
  rq *finished; // A thread that's finished execution

  int threads = MAX_THREADS;

  current = &head;

  while (current) {
    (current->task)();

    if (done[current->tid]) { // Remove from runqueue

      if (threads == 1) // We've finished last one
				return;

      finished = current;
      finished->prev->next = current->next;
      current = current->next;
      current->prev = finished->prev;

      if (current->next == finished) { // Down to last thread
				current->next = finished->next;
      }

      //if (finished != &head)
	//free (finished);
      threads--;
          }
    else
      current = current->next;
  }
}

void init(multiboot_info_t * mbt)
{
	terminal_initialize();
	int i, j;
  for(i=0;i<400000;i++);
  rq *ptr, *pptr;

  /* Build a "runqueue" */
  f[0] = thread1;
  f[1] = thread2;
  f[2] = thread3;

  // Setup runqueue head
  head.task = f[0];
  head.tid = 0;
  head.next = NULL;
  head.prev = NULL;

  // Add any additional threads after the first
  for (j = 1; j < MAX_THREADS; j++) {
    ptr = (rq *) malloc (sizeof (rq));

    if (j == 1) {
      head.next = ptr;
      pptr = &head;
    }
    else {
      pptr = pptr->next;
      pptr->next = ptr;
    }

    ptr->prev = pptr;

    ptr->task = f[j];
    ptr->tid = j;
    ptr->next = &head; // Wraparound
    head.prev = ptr;
  }

  // Let's get going...
  schedule();
  prints("Done 1, 2, 3! \n");
}
