
.global gdt_flush
.global idt_flush


gdt_flush:
	movl 4(%esp), %eax
	lgdt (%eax)
	movw $0x10, %ax
	movw %ax, %ss
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	ljmp $0x8, $1f
1:
	ret

idt_flush:
	movl 4(%esp), %eax
	lidt (%eax)
	ret


.macro ISR_ERRORCODE l
.global isr\l
isr\l:
pushl $0x0
pushl $\l
jmp isr_common_stub
.endm

.irp error, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,13, 14, 15, 16, 17, 18, 19 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50
ISR_ERRORCODE \error
.endr

.global isr32
isr32:
cli
pushl $0x0
pushl $32

isr_common_stub:
	pushal 
	movw %ds, %ax
  pushl %eax             

	movw $0x10, %ax
	movw %ax, %ss
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

  call irq_handler

  popl %ebx     
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %fs
	movw %bx, %gs

  popal       
  addl $0x8, %esp  
	sti
  iret           
