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
	 # start printing
	 movw %ax, %cx 
	 shrw $8, %ax
	 call print
	 movw %cx ,%ax
	 call print
	 jmp final

print:	pushw %dx
	movb %al, %dl
	shrb $4, %al
	cmpb $10, %al
	jge 1f
	addb $0x30, %al
	jmp 2f
1:	addb $55, %al		
2:	movb $0x0E, %ah
	movw $0x07, %bx
	int $0x10

	movb %dl, %al
	andb $0x0f, %al
	cmpb $10, %al
	jge 1f
	addb $0x30, %al
	jmp 2f
1:	addb $55, %al		
2:	movb $0x0E, %ah
	movw $0x07, %bx
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
	.asciz "MemOS: Welcome *** System Memory is: 0x"
msg_finish:
	.asciz " MB."
	.org 0x1FE
	.byte 0x55
	.byte 0xAA
