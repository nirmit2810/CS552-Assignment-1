mkdir image/
gcc -std=gnu99 -c osdev.c -o osdev.o -fno-stack-protector
mount -t ext2 disk.img image/ -o loop,offset=32256
as --32 memos-2.s -o memos-2.o
ld -T memos-2.ld memos-2.o osdev.o -o ./image/boot/memos-2
umount image/
rmdir image/
qemu-system-i386 -m 256 disk.img
