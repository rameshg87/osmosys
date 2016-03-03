# Makefile for smldr

CPPDIR=cpp
ASMDIR=asm
INCL=include
MISCDIR=misc

smldr		:	smldr.o common.o idt.o video.o filesys.o stage3.o
	ld  -T $(MISCDIR)/linker.ld stage3.o video.o smldr.o common.o idt.o filesys.o -o binary/smldr

start.o		:	$(ASMDIR)/stage3.s	
	as $(ASMDIR)/start.s -o start.o

smldr.o		:	$(CPPDIR)/smldr.cpp	
	g++ -c $(CPPDIR)/smldr.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o smldr.o

common.o	:	$(CPPDIR)/common.cpp $(INCL)/common.h
	g++ -c $(CPPDIR)/common.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o common.o

idt.o		:	$(CPPDIR)/idt.cpp $(INCL)/idt.h
	g++ -c $(CPPDIR)/idt.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o idt.o

video.o		:	$(CPPDIR)/video.cpp $(INCL)/video.h
	g++ -c $(CPPDIR)/video.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o video.o

filesys.o    :       $(CPPDIR)/filesys.cpp $(INCL)/filesys.h        
	g++ -c $(CPPDIR)/filesys.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o filesys.o

stage3.o	:	$(ASMDIR)/stage3.s
	as $(ASMDIR)/stage3.s -o stage3.o

stage2		:	stage2.o
	ld stage2.o -o binary/stage2 -Ttext 7E5A --oformat binary

stage2.o	:	$(ASMDIR)/stage2.s
	as $(ASMDIR)/stage2.s -o stage2.o

stage1		:	stage1.o
	ld stage1.o -o binary/stage1 -Ttext 0x7C00 -Tdata 0x7D5A --oformat binary 

stage1.o	:	$(ASMDIR)/stage1.s
	as $(ASMDIR)/stage1.s -o stage1.o


all:
	$(MAKE) stage1
	$(MAKE) stage2
	$(MAKE) smldr
	$(MAKE) clean

clean		:
	rm *.o -f
	rm *~ -f
	rm cpp/*~ -f
	rm include/*~ -f

istage1		:	binary/stage1
	dd if=binary/stage1 of=../disk.img conv=notrunc	

istage2		:	binary/stage2
	dd if=binary/stage2 of=../disk.img conv=notrunc seek=32346 bs=1

istage3		:	binary/smldr
	sudo mount -t vfat /dev/loop0 misc
	cp binary/smldr misc/smldr
	sudo umount misc	

hstage3		:	binary/smldr
	sudo mount -t vfat /dev/sda2 /media/sda2
	cp binary/smldr /media/sda2/
	sudo umount /media/sda2
	partprobe /dev/sda
