gcc -std=gnu99 -c osdev.c -o osdev.o -fno-stack-protector
gcc -std=gnu99 -c output.c -o output.o -fno-stack-protector
gcc -std=gnu99 -c thread.c -o thread.o -fno-stack-protector
gcc -std=gnu99 -c scheduler.c -o scheduler.o -fno-stack-protector
gcc -std=gnu99 -c helper.c -o helper.o -fno-stack-protector
gcc -std=gnu99 -c descriptor_table.c -o descriptor_table.o -fno-stack-protector
gcc -std=gnu99 -c timer.c -o timer.o -fno-stack-protector
gcc -std=gnu99 -c fs.c -o fs.o -fno-stack-protector
gcc -std=gnu99 -c fs_helper.c -o fs_helper.o -fno-stack-protector
gcc -std=gnu99 -c fs_func.c -o fs_func.o -fno-stack-protector
gcc -std=gnu99 -c fs_debug.c -o fs_debug.o -fno-stack-protector
as --32 memos-2.s -o memos-2.o
as --32 switch_thread.s -o switch_thread.o
as --32 -g table.s -o table.o
ld -T memos-2.ld osdev.o switch_thread.o memos-2.o  output.o thread.o scheduler.o descriptor_table.o table.o helper.o timer.o fs.o fs_helper.o fs_func.o fs_debug.o -o memos-2
rm *.o
#objdump -D memos-2 > a
#vim a
qemu-system-i386 -kernel memos-2
