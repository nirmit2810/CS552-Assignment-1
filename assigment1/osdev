	.file	"osdev.c"
	.text
.globl make_color
	.type	make_color, @function
make_color:
	pushl	%ebp
	movl	%esp, %ebp
	movl	12(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	8(%ebp), %eax
	orl	%edx, %eax
	popl	%ebp
	ret
	.size	make_color, .-make_color
.globl make_vgaentry
	.type	make_vgaentry, @function
make_vgaentry:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movb	%dl, -20(%ebp)
	movb	%al, -24(%ebp)
	movsbw	-20(%ebp),%ax
	movw	%ax, -2(%ebp)
	movzbl	-24(%ebp), %eax
	movw	%ax, -4(%ebp)
	movzwl	-4(%ebp), %eax
	sall	$8, %eax
	movl	%eax, %edx
	movzwl	-2(%ebp), %eax
	orl	%edx, %eax
	leave
	ret
	.size	make_vgaentry, .-make_vgaentry
.globl strlen
	.type	strlen, @function
strlen:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L6
.L7:
	addl	$1, -4(%ebp)
.L6:
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L7
	movl	-4(%ebp), %eax
	leave
	ret
	.size	strlen, .-strlen
	.section	.rodata
	.align 4
	.type	VGA_WIDTH, @object
	.size	VGA_WIDTH, 4
VGA_WIDTH:
	.long	80
	.align 4
	.type	VGA_HEIGHT, @object
	.size	VGA_HEIGHT, 4
VGA_HEIGHT:
	.long	25
	.comm	terminal_row,4,4
	.comm	terminal_column,4,4
	.comm	terminal_color,1,1
	.comm	terminal_buffer,4,4
	.text
.globl terminal_initialize
	.type	terminal_initialize, @function
terminal_initialize:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$24, %esp
	movl	$0, terminal_row
	movl	$0, terminal_column
	movl	$0, 4(%esp)
	movl	$7, (%esp)
	call	make_color
	movb	%al, terminal_color
	movl	$753664, terminal_buffer
	movl	$0, -8(%ebp)
	jmp	.L10
.L13:
	movl	$0, -12(%ebp)
	jmp	.L11
.L12:
	movl	VGA_WIDTH, %eax
	imull	-8(%ebp), %eax
	addl	-12(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	terminal_buffer, %eax
	movl	-16(%ebp), %edx
	addl	%edx, %edx
	leal	(%eax,%edx), %ebx
	movzbl	terminal_color, %eax
	movzbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$32, (%esp)
	call	make_vgaentry
	movw	%ax, (%ebx)
	addl	$1, -12(%ebp)
.L11:
	movl	VGA_WIDTH, %eax
	cmpl	%eax, -12(%ebp)
	jb	.L12
	addl	$1, -8(%ebp)
.L10:
	movl	VGA_HEIGHT, %eax
	cmpl	%eax, -8(%ebp)
	jb	.L13
	addl	$24, %esp
	popl	%ebx
	popl	%ebp
	ret
	.size	terminal_initialize, .-terminal_initialize
.globl terminal_setcolor
	.type	terminal_setcolor, @function
terminal_setcolor:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	8(%ebp), %eax
	movb	%al, -4(%ebp)
	movzbl	-4(%ebp), %eax
	movb	%al, terminal_color
	leave
	ret
	.size	terminal_setcolor, .-terminal_setcolor
.globl terminal_putentryat
	.type	terminal_putentryat, @function
terminal_putentryat:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$32, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movb	%dl, -24(%ebp)
	movb	%al, -28(%ebp)
	movl	VGA_WIDTH, %eax
	imull	20(%ebp), %eax
	addl	16(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	terminal_buffer, %eax
	movl	-8(%ebp), %edx
	addl	%edx, %edx
	leal	(%eax,%edx), %ebx
	movzbl	-28(%ebp), %edx
	movsbl	-24(%ebp),%eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	make_vgaentry
	movw	%ax, (%ebx)
	addl	$32, %esp
	popl	%ebx
	popl	%ebp
	ret
	.size	terminal_putentryat, .-terminal_putentryat
.globl terminal_putchar
	.type	terminal_putchar, @function
terminal_putchar:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$20, %esp
	movl	8(%ebp), %eax
	movb	%al, -8(%ebp)
	movl	terminal_row, %ebx
	movl	terminal_column, %ecx
	movzbl	terminal_color, %eax
	movzbl	%al, %edx
	movsbl	-8(%ebp),%eax
	movl	%ebx, 12(%esp)
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	terminal_putentryat
	movl	terminal_column, %eax
	addl	$1, %eax
	movl	%eax, terminal_column
	movl	terminal_column, %edx
	movl	VGA_WIDTH, %eax
	cmpl	%eax, %edx
	jne	.L21
	movl	$0, terminal_column
	movl	terminal_row, %eax
	addl	$1, %eax
	movl	%eax, terminal_row
	movl	terminal_row, %edx
	movl	VGA_HEIGHT, %eax
	cmpl	%eax, %edx
	jne	.L21
	movl	$0, terminal_row
.L21:
	addl	$20, %esp
	popl	%ebx
	popl	%ebp
	ret
	.size	terminal_putchar, .-terminal_putchar
.globl terminal_writestring
	.type	terminal_writestring, @function
terminal_writestring:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	strlen
	movl	%eax, -4(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L23
.L24:
	movl	-8(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	movsbl	%al,%eax
	movl	%eax, (%esp)
	call	terminal_putchar
	addl	$1, -8(%ebp)
.L23:
	movl	-8(%ebp), %eax
	cmpl	-4(%ebp), %eax
	jb	.L24
	leave
	ret
	.size	terminal_writestring, .-terminal_writestring
	.section	.rodata
.LC0:
	.string	"Hello, kernel World!\n"
	.text
.globl kernel_main
	.type	kernel_main, @function
kernel_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	call	terminal_initialize
	movl	$.LC0, (%esp)
	call	terminal_writestring
	leave
	ret
	.size	kernel_main, .-kernel_main
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits
