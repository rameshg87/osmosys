/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#ifndef IDT_DOT_H

#define IDT_DOT_H

#define TIME_QUANTUM 2

//structure for an idt entry
struct idt_entry
{
  unsigned short int baselow;		//B15-B0
  unsigned short int selector;
  unsigned char always0;
  unsigned char flags;
  unsigned short int basehigh;		//B31-B15
}__attribute__((packed));

struct idt_ptr
{
  unsigned short limit;
  idt_entry *base;
}__attribute__((packed));

struct reg_val
{
	unsigned int gs,fs,es,ds;
	unsigned int edi,esi,ebp,esp,ebx,edx,ecx,eax;
	unsigned int int_no,err_code;
	unsigned int eip,cs,eflags,user_esp,ss;
};

struct scancode_list
{
	char scancode;
	struct scancode_list *next;
	struct scancode_list *prev;
};

void idt_install ();
void idt_setup ();
void irqremap ();
void keyboard_handler ();
extern "C" void loadidt ();


#endif
