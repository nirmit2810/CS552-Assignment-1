#ifndef __H_HELPER__
#define __H_HELPER__
#include "stdint.h"
#include "global_t.h"

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}


static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline void io_wait(void)
{
    /* TODO: This is probably fragile. */
    asm volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}

void strcpy_b (char * destination, const char * source, uint8_t byte);
bool strmatch (const char * str1, const char * str2);
#endif
