/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#ifndef MEMORY_DOT_H
#define MEMORY_DOT_H
#include "common.h"
#include "video.h"


#define BITMAP_SIZE 131072
#define STACK_SIZE 512
#define RESERVED_FRAMES 0x800
#define RESERVED_ALLOCATION_START 0x600
#define PAGE_SIZE 0x1000


#define KERNEL_LOGICAL_MEMORY	1
#define USER_LOGICAL_MEMORY	0

class physical_memory
{
	unsigned short int total_memory;
	unsigned int stack[STACK_SIZE];
	unsigned short int stack_top;
	unsigned char bitmap[BITMAP_SIZE];
	
	unsigned int last_found_character;
	unsigned short int rlast_found_character;
	
public:
	unsigned int max_frames;
	void initialize ();
	unsigned int rallocate ();
	void rdeallocate (unsigned int);
	unsigned int allocate();
	void deallocate(unsigned int);
	unsigned int search();  
};

struct kdm_heap_entry 
{
  unsigned short int offset_in_page:12;
  unsigned short int size_of_block:12;
}__attribute__((packed));

struct kdm_page_attribute
{
	unsigned short int vhp;
	unsigned short int count;
}__attribute__((packed));

class logical_memory
{
	unsigned int dynamic_memory_start;
	unsigned int *page_directory;
	
public:
	unsigned int kdm_end_pointer;
	void initialize_memory (unsigned short int);
	void map (unsigned int,unsigned int,unsigned short int);
	void mmap (unsigned int page_address1,logical_memory *destination,unsigned int page_address2); 
	void load_page_directory ();
	void* kdmalloc (unsigned int);
	int get_page_attributes (unsigned int);
	void set_page_attribute (unsigned int,unsigned short int);
	void kdfree (void *,unsigned int);
	unsigned int get_physical_address (unsigned int);
	void display ();
	void print_heap_details (unsigned int addr);
	unsigned int search_heap (unsigned int,unsigned int);
	unsigned int *get_page_directory_address ();
};

void * operator new (unsigned int);

#endif
