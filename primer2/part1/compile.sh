as --32 memos-1.s -o memos-1.o
ld -T memos-1.ld memos-1.o -o memos1
dd bs=1 if=memos1 of=memos1_test skip=0 count=512
qemu-system-i386 -m 32 memos1_test
