	.globl _start
	.code16
_start:

	# setting up segment
	movw $0, %ax
	movw %ax, %ds

	# start to print string
	leaw msg, %si
char_loop:
	lodsb
 testb %al, %al
 jz done_beginning
	# interrupting for output
	movb $0x0E, %ah
	int $0x10
	jmp char_loop
done_beginning:

# Memory starts here
	 movw $0xE801, %ax
	 int $0x15
	 addw $1024, %ax
	 shrw $10, %ax
	 shrw $4, %bx
	 addw %bx, %ax
	 # start printing in decimal
	 movw $0, %dx 
	 movw $0, %bx     
	 movw $10000, %cx
	 div %cx
	 cmp $0, %ax
	 jz 1f
	 movw $1, %bx
	 call print
1:	 movw %dx, %ax
	 movw $1000, %cx
	 movw $0, %dx 
	 div %cx
	 cmp $0, %bx
	 jnz 6f
	  cmp $0, %ax
	 jz 2f
	 movw $1, %bx
6:	 call print
2:	 movw %dx, %ax
	 movw $100, %cx
	 movw $0, %dx
	 div %cx
	  cmp $0, %bx
	 jnz 7f
	  cmp $0, %ax
	 jz 3f
	 movw $1, %bx
7:	 call print
3:	 movw %dx, %ax
	 movw $10, %cx
	 movw $0, %dx
	 div %cx
	 cmp $0, %bx
	 jnz 8f
	  cmp $0, %ax
	 jz 3f
	 movw $1, %bx
8:	 call print
3:	 movw %dx, %ax
	 call print
	 jmp final

print:	pushw %dx
	
	addb $0x30, %al
	movb $0x0E, %ah
	int $0x10
	popw %dx
	ret

final:
	# start to print string
	leaw msg_finish, %si
	char_loop_finish:
	lodsb
	testb %al, %al
    jz done_beginning_finish
	# interrupting for output
	movb $0x0E, %ah
	int $0x10
	jmp char_loop_finish
done_beginning_finish:

msg: 
	.asciz "MemOS: Welcome *** System Memory is: "
msg_finish:
	.asciz " MB."
	.org 0x1FE
	.byte 0x55
	.byte 0xAA
