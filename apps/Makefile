idlep	:	p1.o	test.o
		ld --oformat binary p1.o test.o -Ttext 0x2000000
		echo "\xff\xff\x0f\x00" | dd of=idlep 
		dd if=a.out of=idlep seek=4 bs=1 conv=notrunc
		mount /dev/loop0 misc
		cp idlep misc/bin/
		umount /dev/loop0

p1.o	:	p1.s
		as p1.s -o p1.o

test.o	:	test.c syscalls.h
		gcc -c test.c -fno-builtin -nostdlib

shell	:	p1.o	shell.o
		ld --oformat binary p1.o shell.o -Ttext 0x2000000
		echo "\xff\xff\x0f\x00" | dd of=shell 
		dd if=a.out of=shell seek=4 bs=1 conv=notrunc
		mount /dev/loop0 misc
		cp shell misc/bin/
		umount /dev/loop0

shell.o	:	shell.c syscalls.h
		gcc -c shell.c -fno-builtin -nostdlib

ls	:	p1.o	ls.o
		ld --oformat binary p1.o ls.o -Ttext 0x2000000
		echo "\xff\xff\x0f\x00" | dd of=shell 
		dd if=a.out of=ls seek=4 bs=1 conv=notrunc
		mount /dev/loop0 misc
		cp ls misc/bin/
		umount /dev/loop0

ls.o	:	ls.c syscalls.h
		gcc -c ls.c -fno-builtin -nostdlib

texted	:	p1.o	texted.o
		ld p1.o texted.o -T linker.ld 
		echo "\xff\xff\x0f\x00" | dd of=texted 
		dd if=a.out of=texted seek=4 bs=1 conv=notrunc
		mount /dev/loop0 misc
		cp texted misc/bin/
		umount /dev/loop0

texted.o	:	texted.c syscalls.h
		gcc -c texted.c -fno-builtin -nostdlib
