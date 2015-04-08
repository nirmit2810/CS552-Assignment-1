#ifndef __H_TIMER__
#define __H_TIMER__
#include "stdint.h"

typedef struct registers
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;

void init_timer();
void irq_handler(registers_t regs);
void isr_handler(registers_t regs);

#endif

