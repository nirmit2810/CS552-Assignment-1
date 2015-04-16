gcc -std=gnu99 -c osdev.c -o osdev.o -fno-stack-protector
as --32 memos-2.s -o memos-2.o
ld -T memos-2.ld memos-2.o osdev.o -o memos-2
rm *.o
qemu-system-i386 -kernel memos-2
