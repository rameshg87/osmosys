# Makefile for osmosys

CPPDIR=cpp
ASMDIR=asm
INCL=include
MISCDIR=misc

kernel		:	main.o common.o gdt.o idt.o video.o memory.o start.o storage.o filesystem.o filemanager.o process.o syscalls.o cdfs.o
	ld  -T $(MISCDIR)/linker.ld start.o main.o gdt.o idt.o common.o video.o memory.o storage.o filesystem.o filemanager.o process.o syscalls.o cdfs.o -o binary/kernel

start.o		:	$(ASMDIR)/start.s	
	as $(ASMDIR)/start.s -o start.o

main.o		:	$(CPPDIR)/main.cpp
	g++-4.2 -c $(CPPDIR)/main.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o main.o

common.o	:	$(CPPDIR)/common.cpp
	g++-4.2 -c $(CPPDIR)/common.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o common.o

gdt.o		:	$(CPPDIR)/gdt.cpp
	g++-4.2 -c $(CPPDIR)/gdt.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o gdt.o

idt.o		:	$(CPPDIR)/idt.cpp
	g++-4.2 -c $(CPPDIR)/idt.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o idt.o

video.o		:	$(CPPDIR)/video.cpp
	g++-4.2 -c $(CPPDIR)/video.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o video.o

memory.o	:	$(CPPDIR)/memory.cpp
	g++-4.2 -c $(CPPDIR)/memory.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o memory.o

storage.o    :       $(CPPDIR)/storage.cpp $(INCL)/storage.h
	g++-4.2 -c $(CPPDIR)/storage.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o storage.o


filesystem.o    :       $(CPPDIR)/filesystem.cpp        
	g++-4.2 -c $(CPPDIR)/filesystem.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o filesystem.o


filemanager.o	:	$(CPPDIR)/filemanager.cpp	$(INCL)/filemanager.h
	g++-4.2 -c $(CPPDIR)/filemanager.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o filemanager.o

process.o    :       $(CPPDIR)/process.cpp        
	g++-4.2 -c $(CPPDIR)/process.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o process.o


syscalls.o    :       $(CPPDIR)/syscalls.cpp
	g++-4.2 -c $(CPPDIR)/syscalls.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o syscalls.o

cdfs.o :	 $(CPPDIR)/cdfs.cpp        
	g++-4.2 -c $(CPPDIR)/cdfs.cpp -I$(INCL) -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o cdfs.o





install		:	binary/kernel
	mount -t vfat /dev/loop0 misc
	cp binary/kernel misc/osmosys/kernel
	umount misc

install2	:	binary/kernel
	mount -t vfat /dev/sda1 misc
	cp binary/kernel misc/osmosys/kernel
	umount misc
	partprobe /dev/sda

clean		:
	rm *.o -f
	rm *~ -f
	rm cpp/*~ -f
	rm include/*~ -f
	rm *~ -rf	

