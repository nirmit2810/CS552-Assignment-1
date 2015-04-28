#include "timer.h"
#include "helper.h"
#include "output.h"
#include "thread.h"

//void init_timer(uint32_t frequency)
void init_timer()
{

   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   //uint32_t divisor = 1193180 / frequency;

   // Send the command byte.
	 //divisor = 1193181 * 4;
  outb(0x43, 0x34);
	io_wait();

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   //uint8_t l = (uint8_t)(divisor & 0xFF);
   //uint8_t h = (uint8_t)( (divisor>>8) & 0xFF);

   // Send the frequency divisor.
   //outb(0x40, l);
   //outb(0x40, h);
   outb(0x40, 0xFF);
   outb(0x40, 0xFF);
	asm volatile("sti\n\t");
}


// This gets called from our ASM interrupt handler stub.
void irq_handler(registers_t regs)
{
   // Send an EOI (end of interrupt) signal to the PICs.
   // If this interrupt involved the slave.
   if (regs.int_no >= 40)
   {
       outb(0xA0, 0x20);
   }
   outb(0x20, 0x20);
	io_wait();

	 isr_handler(regs);
}

void isr_handler(registers_t regs)
{
	schedule();
}
