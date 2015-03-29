gcc -std=gnu99 -c osdev.c -o osdev.o -fno-stack-protector
mount -t ext2 disk.img image/ -o loop,offset=32256
as --32 fifo.s -o fifo.o
ld -T fifo.ld fifo.o osdev.o -o /root/my-documents/new/repo-os-master/part2/image/boot/fifo
umount image/
qemu-system-i386 -m 256 disk.img
