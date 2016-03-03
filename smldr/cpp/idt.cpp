/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.2

********************************************************************************/

#include <idt.h>
#include <video.h>
#include <common.h>

struct idt_entry idt_entries [256];
extern "C" void timerhandler ();
extern "C" void keyboardhandler ();
extern "C" void commonhandler ();
extern oses os[];
extern int current_option;
int timer=0;
bool is_timer_active;
extern int current_option;
extern int oscount;
extern void print_options ();
extern void bootosmosys (int);
extern void chainload_os (int);

#define UP_ARROW	0x48
#define DOWN_ARROW	0x50
#define ENTER_KEY	0x1C

extern "C" void irq0 ();

void idt_install ()
{
  irqremap ();

  //setup descriptor table
  idt_setup ();

  //load the idtr register
  loadidt ();
}

void exception_handler ()
{
  putstring ("Exception encountered - Unable to proceed\n");
}

extern "C" void timer_handler ()
{
  if (is_timer_active == true)
  {
    timer --;
    if (timer == 0)
    {
      if (os[current_option].isosmosys == true)
	{
          outportb(0x20, 0x20);	
          bootosmosys (os[current_option].partindex);
	}
      else
	{ 
          outportb(0x20, 0x20);	
	  chainload_os (os[current_option].partindex);
	}


    }
	print_options ();
  }
  outportb(0x20, 0x20);
}

extern "C" void keyboard ()
{
  int scancode = inportb (0x60);
  is_timer_active = false;
  if (!(scancode & 0x80))
  {
    if (scancode == DOWN_ARROW)
    {
      current_option++;
      if (current_option == oscount)
        current_option = 0;
      print_options ();
    }
    else if (scancode == UP_ARROW)
    {
      current_option--;
      if (current_option == -1)
        current_option = oscount-1;
      print_options ();
    }
    else if (scancode == ENTER_KEY)
    {
      if (os[current_option].isosmosys == true)
	{
          outportb(0x20, 0x20);	
          bootosmosys (os[current_option].partindex);
	}
      else
	{ 
          outportb(0x20, 0x20);	
	  chainload_os (os[current_option].partindex);
	}
    }
    

  }
          outportb(0x20, 0x20);	
}

extern "C" void common ()
{
	outportb (0x20,0x20);
	outportb (0xa0,0x20);
}

void fill_idt_entry (struct idt_entry &anentry,unsigned int base)
{
  // fill idt entry given the base 
  anentry.baselow = ((unsigned int)(base)) & 0xFFFF;
  anentry.basehigh = 0xFFFF & (((unsigned int)(base)) >> 16);

  // segment selector is the code segment for the kernel
  anentry.selector = 0x08;

  // flags 
  anentry.flags = 0x8e;
  anentry.always0 = 0;

}

void idt_setup ()
{
  int i;

  for (i=0;i<32;i++)
    fill_idt_entry (idt_entries[i],(unsigned int)&exception_handler);

  for (i=0x70;i<0x78;i++)
    fill_idt_entry (idt_entries[i],(unsigned int)&commonhandler);

  fill_idt_entry (idt_entries[8],(unsigned int)&timerhandler);
  fill_idt_entry (idt_entries[9],(unsigned int)&keyboardhandler);

}

void irqremap()
{
  /* ICW1 - port 0x20 is master pic and port 0xa0 is slave pic */
  
  outportb(0x20,0x11);
  outportb(0xA0,0x11);

  /* ICW2 - port 0x21 is master pic and port 0xa1 is slave pic 
     remapping is done here */
  outportb(0x21,0x08);
  outportb(0xA1,0x70);

  /* ICW3  select slave pin for master & set ID for slave*/ 

  outportb(0x21,0x04);
  outportb(0xA1,0x02);

  /* ICW4 - set 8086 mode */

  outportb(0x21,0x01);
  outportb(0xA1,0x01);

  /* Stop Initialization */

  outportb(0x21,0x0);
  outportb(0xA1,0x0);
}
