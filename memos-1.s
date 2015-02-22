	.globl _start
	.code16
_start:
	# setting up segment
	movw $0x07C0, %ax
	movw %ax, %ds
	cld

	# start to print string
	movw msg, %si
#	movl msg, %esi
char_loop:
	lodsb
#	testb %al, %al
#	jz done
	# interrupting for output

	movb $0x0E, %ah
	int $0x10
#	jmp char_loop

done:

msg: 
	.asciz "Welcome"
	len = . - msg			# length

	.org 0x1FE

	.byte 0x55
	.byte 0xAA

# To test:	
# as --32 memos-1.s -o memos-1.o
# ld -T memos-1.ld memos-1.o -o memos1
# dd bs=1 if=memos1 of=memos1_test skip=4096 count=512
# qemu-system-i386 memos1_test
