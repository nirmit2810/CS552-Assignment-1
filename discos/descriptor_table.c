#include "descriptor_table.h"
#include "output.h"
#include "helper.h"

#define GDT_NUM_ENTRIES 3
#define IDT_NUM_ENTRIES 33 
gdt_entry_t gdt_entries[GDT_NUM_ENTRIES];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[IDT_NUM_ENTRIES];
idt_ptr_t   idt_ptr;

extern void gdt_flush();
extern void idt_flush();

void init_tables(){
	init_gdt();
	PIC_remap(0x20, 0x28);
	init_idt();
}
/*

This code is taken from
http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html

*/

static void init_gdt()
{
   gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_NUM_ENTRIES) - 1;
   gdt_ptr.base  = (uint32_t)&gdt_entries;

   gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
   gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
   gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
   gdt_flush((uint32_t)&gdt_ptr);
}

// Set the value of one GDT entry.
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
   gdt_entries[num].base_low    = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high   = (base >> 24) & 0xFF;

   gdt_entries[num].limit_low   = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access      = access;
}

 
#define set_gate(X) extern void isr##X();idt_set_gate( X, (uint32_t)isr##X , 0x08, 0x8E); 

static void init_idt()
{
   idt_ptr.limit = sizeof(idt_entry_t) * IDT_NUM_ENTRIES -1;
   idt_ptr.base  = (uint32_t)&idt_entries;

	//CLEAR EVEYRTHING IN CASE THERE's SOMETHING
	for(int i = 0; i < sizeof(idt_entry_t) * IDT_NUM_ENTRIES; i++)
		*((uint8_t*) (    ((uint32_t)&idt_entries) + i)) = 0;

	set_gate(0);
	set_gate(1);
	set_gate(2);
	set_gate(3);
	set_gate(4);
	set_gate(5);
	set_gate(6);
	set_gate(7);
	set_gate(8);
	set_gate(9);
	set_gate(10);
	set_gate(11);
	set_gate(12);
	set_gate(13);
	set_gate(14);
	set_gate(15);
	set_gate(16);
	set_gate(17);
	set_gate(18);
	set_gate(19);
	set_gate(20);
	set_gate(21);
	set_gate(22);
	set_gate(23);
	set_gate(24);
	set_gate(25);
	set_gate(26);
	set_gate(27);
	set_gate(28);
	set_gate(29);
	set_gate(30);
	set_gate(31);
	set_gate(32);

  idt_flush((uint32_t)&idt_ptr);
	//asm volatile("sti\n\t");
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   idt_entries[num].flags   = flags ;
}

 
/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}
