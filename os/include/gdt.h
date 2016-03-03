/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#ifndef GDT_DOT_H

#define GDT_DOT_H


struct gdt_entry
{
  unsigned short int limit_low;
  unsigned short int base_low;
  unsigned char base_middle;
  unsigned char access;
  unsigned char granularity;
  unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr
{
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

gdt_entry gdt_assign(unsigned int limit,unsigned int base,unsigned char access,unsigned char granularity);
void gdt_setup();

// A struct describing a Task State Segment.
struct tss_entry_struct
{
   unsigned int prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   unsigned int esp0;       // The stack pointer to load when we change to kernel mode.
   unsigned int ss0;        // The stack segment to load when we change to kernel mode.
   unsigned int esp1;       // Unused...
   unsigned int ss1;
   unsigned int esp2;
   unsigned int ss2;
   unsigned int cr3;
   unsigned int eip;
   unsigned int eflags;
   unsigned int eax;
   unsigned int ecx;
   unsigned int edx;
   unsigned int ebx;
   unsigned int esp;
   unsigned int ebp;
   unsigned int esi;
   unsigned int edi;
   unsigned int es;         // The value to load into ES when we change to kernel mode.
   unsigned int cs;         // The value to load into CS when we change to kernel mode.
   unsigned int ss;         // The value to load into SS when we change to kernel mode.
   unsigned int ds;         // The value to load into DS when we change to kernel mode.
   unsigned int fs;         // The value to load into FS when we change to kernel mode.
   unsigned int gs;         // The value to load into GS when we change to kernel mode.
   unsigned int ldt;        // Unused...
   unsigned short int trap;
   unsigned short int iomap_base;
} __attribute__((packed));

typedef struct tss_entry_struct tss_entry_t; 


#endif
