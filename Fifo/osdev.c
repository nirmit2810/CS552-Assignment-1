#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "multiboot.h"
#define crBegin static int state=0; switch(state) { case 0:

/*
 * ANSI C provides the __LINE__ macro to return the current source line number
 * We can use the line number as a simple form of programmatic state!
 */
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
/*

 This CODE IS TAKEN DIRECLTY FROM OSDEV.ORG, which was recommended by Professor West.
 I WENT THROUGH tHE CODE TO MAKE SURE THAT I UNDERSTOOD EVERYTHING..
 */
/* Hardware text mode color constants. */
enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};
 
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}
 
uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}
 
size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_RED, COLOR_WHITE);
	terminal_buffer = (uint16_t*) 0xB8000;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}
 
void terminal_putchar(char c)
{   if(c == '\n'){
	terminal_column = 0;
	++terminal_row;
}else{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if ( ++terminal_column == VGA_WIDTH )
	{
		terminal_column = 0;
		if ( ++terminal_row == VGA_HEIGHT )
		{
			terminal_row = 0;
		}
	}
}
}
 
void terminal_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		terminal_putchar(data[i]);
}

char * itoa( multiboot_uint32_t value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

int thread1 (void) {

  int i;
  static int j;

  crBegin;
  while (1) {
    for (i = 0; i < 10; i++) {

          terminal_writestring("<1>");
        // usleep (100000);
      //fflush (stdout);
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
      //usleep (100000);
      //fflush (stdout);
    }
     terminal_writestring("\n");
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
 //     usleep (100000); 	
      //fflush (stdout);
    }
     terminal_writestring("\n");
    crReturn (2); // Time to yield

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


 
void kernel_main(multiboot_info_t * mbt, unsigned int magic)
{
	terminal_initialize();
  int i;
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
  for (i = 1; i < MAX_THREADS; i++) {
    ptr = (rq *) malloc (sizeof (rq));

    if (i == 1) {
      head.next = ptr;
      pptr = &head;
    }
    else {
      pptr = pptr->next;
      pptr->next = ptr;
    }

    ptr->prev = pptr;

    ptr->task = f[i];
    ptr->tid = i;
    ptr->next = &head; // Wraparound
    head.prev = ptr;
  }

  // Let's get going...
  schedule ();
  
  terminal_writestring("Done 1, 2, 3! \n");

}



