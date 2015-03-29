.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
_start:
        jmp real_start

        /* Multiboot header -- Safe to place this header in 1st page of memory for GRUB */
        .align 4
        .long 0x1BADB002 /* Multiboot magic number */
        .long 0x00000003 /* Align modules to 4KB, req. mem size */
                         /* See 'info multiboot' for further info */
        .long 0xE4524FFB /* Checksum */

real_start: 

        # This is where the rest of your program goes
				movl $stack_top, %esp
				pushl %ebx
				call kernel_main
				cli
				hlt
.loop:
			jmp .loop

.size _real_start, . - _real_start
