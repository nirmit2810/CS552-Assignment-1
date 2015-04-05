
.global gdt_flush


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
