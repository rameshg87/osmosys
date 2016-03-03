/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.2

********************************************************************************/


#ifndef IDT_DOT_H

#define IDT_DOT_H


//structure for an idt entry
struct idt_entry
{
  unsigned short int baselow;		//B15-B0
  unsigned short int selector;
  unsigned char always0;
  unsigned char flags;
  unsigned short int basehigh;		//B31-B15
};

// __attribute__((packed));



void idt_install ();
void idt_setup ();
void irqremap ();
void keyboard_handler ();
extern "C" void loadidt ();

#endif
