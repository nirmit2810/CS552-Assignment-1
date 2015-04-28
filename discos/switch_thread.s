
	.global switch_thread
.text

switch_thread:
	// Test if esi is null (no source)
	testl %esi, %esi 
	jz 1f
	//Push all flags
	pushfl 
	//Push all registers
	pushal 

	//Pushing segment selector onto stacks

	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs

	//Saving stack pointer to TCB ( first entry in structure)
	movl %esp, 0x0(%esi)

1:
	//Getting new stack pointer
	movl 0x0(%edi), %esp
 // Poping in reverse order
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popal
	popfl

	ret



