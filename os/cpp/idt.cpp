/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
Version 1.0.0
Last updated - 18/01/09

********************************************************************************/

#include <idt.h>
#include <common.h>
#include <video.h>
#include <syscalls.h>
#include<process.h>

extern display disp1;
extern scheduler slr;
extern struct idt_entry idt_entries [256];
extern struct idt_ptr iptr;

extern "C" void timerisr ();
extern "C" void keyboardisr ();
extern "C" void load_idt (unsigned int);
extern "C" void primarydiskisr ();
extern "C" void secondarydiskisr ();
extern "C" void syscallisr ();
extern "C" void floppyisr ();
extern "C" void exception0 ();
extern "C" void exception1 ();
extern "C" void exception2 ();
extern "C" void exception3 ();
extern "C" void exception4 ();
extern "C" void exception5 ();
extern "C" void exception6 ();
extern "C" void exception7 ();
extern "C" void exception8 ();
extern "C" void exception9 ();
extern "C" void exception10 ();
extern "C" void exception11 ();
extern "C" void exception12 ();
extern "C" void exception13 ();
extern "C" void exception14 ();
extern "C" void exception15 ();
extern "C" void exception16 ();
extern "C" void exception17 ();
extern "C" void exception18 ();
extern "C" void exception19 ();
extern "C" void exception20 ();
extern "C" void exception21 ();
extern "C" void exception22 ();
extern "C" void exception23 ();
extern "C" void exception24 ();
extern "C" void exception25 ();
extern "C" void exception26 ();
extern "C" void exception27 ();
extern "C" void exception28 ();
extern "C" void exception29 ();
extern "C" void exception30 ();
extern "C" void exception31 ();
extern unsigned char round_robin_start;

extern int irq6_flag;
extern int irq14_flag;
extern int irq15_flag;
extern int irq1_flag;
extern int irq1_status;

extern logical_memory klm;
extern unsigned long sys_clock;
extern unsigned int process_clock;
extern char scancode;
extern scancode_list *schead,*sctail;
extern int current_pid;
extern bool caps;


void insert_into_scancode (char code)
{
  if (schead == 0)
  {
	  
    schead = (scancode_list*)klm.kdmalloc (sizeof (scancode_list));
    schead -> next = 0;
    schead -> prev = 0;            
    sctail = schead;
    schead -> scancode = code;
  }
  else
  {
	sctail -> next = (scancode_list*)klm.kdmalloc (sizeof (scancode_list));
    sctail -> next -> prev = sctail;
    sctail = sctail -> next;
    sctail -> scancode = code;
    sctail -> next = 0;    
  }
}


void idt_install ()
{
    /* remap the timer to 1.19Mhz */
    int divisor = 1193180;
    outportb(0x43, 0x36); 
    outportb(0x40, divisor & 0xFF);   
    outportb(0x40, divisor >> 8);

  //remap irqs to idt entries 32-47
  irqremap();

  //setup descriptor table
  idt_setup ();

  //load the idtr register
  load_idt ((unsigned int)(&iptr));
}

extern "C" void exception_handler (struct reg_val *r)
{
  disp1.setattrib (0x0c);

  switch(r->int_no)
	{
		case 0:disp1.putstring ("Divide by 0 exception\n");break;
		case 1:disp1.putstring ("Debug exception\n");break;
		case 2:disp1.putstring ("Non maskable interrupt exception\n");break;
		case 3:disp1.putstring ("Breakpoint exception\n");break;
		case 4:disp1.putstring ("Into detected overflow exception\n");break;
		case 5:disp1.putstring ("Out of bounds exception\n");break;
		case 6:disp1.putstring ("Invalid opcode exception\n");break;
		case 7:disp1.putstring ("No coprocessor exception\n");break;
		case 8:disp1.putstring ("Double fault exception\n");break;
		case 9:disp1.putstring ("Coprocessor segment overrun exception\n");break;
		case 10:disp1.putstring ("Bad TSS exception\n");break;
		case 11:disp1.putstring ("Segment not present exception\n");break;
		case 12:disp1.putstring ("Stack fault exception\n");break;
		case 13:disp1.putstring ("General protection fault exception\n");break;
		case 14:disp1.putstring ("Page fault exception\n");
			unsigned int faulty_address;
			__asm__ ("mov %%cr2,%%eax" : "=a"(faulty_address):);
			disp1.printf ("\nFaulty Address = %x\n",faulty_address);
			break;
		case 15:disp1.putstring ("Unknown interrupt exception\n");break;
		case 16:disp1.putstring ("Coprocessor fault exception\n");break;
		case 17:disp1.putstring ("Alignment check exception\n");break;
		case 18:disp1.putstring ("Machine check exception\n");break;
		default:disp1.putstring ("Reserved exceptions\n");
	}

	disp1.printf("EAX = %x\n",r->eax);
	disp1.printf("EBX = %x\n",r->ebx);
	disp1.printf("ECX = %x\n",r->ecx);
	disp1.printf("EDX = %x\n",r->edx);
	disp1.printf("ESP = %x\n",r->esp);
	disp1.printf("EBP = %x\n",r->ebp);
	disp1.printf("USER ESP = %x\n",r->user_esp);
	disp1.printf("EIP = %x\n",r->eip);
	for (;;);
}


extern "C" void timerhandler (struct reg_val *reg)
{	
  outportb(0x20, 0x20);

  	
  sys_clock++;
  
  if (round_robin_start)process_clock++;
	
	if(process_clock==TIME_QUANTUM)
	{
		if (slr.running_list_element != 0)
		{
			process_clock=0;
			slr.running_list_element -> ptr -> put_kernel_stack_value (reg);
			slr.process_switch(-1);
		}
	}
  
}

extern "C" void keyboardhandler (struct reg_val *r)
{
	scancode= inportb (0x60);
	{if(scancode==CAPS){if(caps==0)caps=1;else caps=0;}}
	if(irq1_flag<0)
	{
		if (!(scancode & 0x80))
		{
			//if(scancode == CAPS) caps=~caps;
		}
		outportb(0x20, 0x20);
	}
	else
	{      
		insert_into_scancode (scancode);
		int callswitch = irq1_status;
		irq1_status=1;
//		disp1.printf ("{%x}",scancode&0xff);
		process_clock=0;
		outportb(0x20, 0x20);
		if (callswitch == 0)
			slr.process_switch(irq1_flag);
	}
}

void fill_idt_entry (struct idt_entry &anentry,unsigned int base)
{
  // fill idt entry given the base 
  anentry.baselow = ((unsigned int)(base)) & 0xFFFF;
  anentry.basehigh = 0xFFFF & (((unsigned int)(base)) >> 16);

  // segment selector is the code segment for the kernel
  anentry.selector = 0x08;

  // flags 
  anentry.flags = 0xee;
  anentry.always0 = 0;

}


extern "C" void primarydiskhandler()
{
  outportb(0xA0, 0x20);
  outportb(0x20, 0x20);

  if (irq14_flag == -2)
  {
    irq14_flag = -1;
  }
  else if (round_robin_start == 0)
  {
    unsigned char status = inportb (0x1f7);

    if ( status & 0x1 )
    {
      unsigned char error = inportb (0x1f1);
      irq14_flag = (unsigned int)error & 0xff;
      disp1.printf ("2disk controller i/o error,error=%x\n",error);
    }
 
    else if (((status & 0x80) == 0) && ((status & 0x08) != 0))
      irq14_flag=1;       
  }
  else if (round_robin_start == 1 && irq14_flag != -1)  
  {
    inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);unsigned char status = inportb (0x1f7);

    if ( status & 0x1 )
    {
      unsigned char error = inportb (0x1f1);
      irq14_flag = (unsigned int)error & 0xff;
      disp1.printf ("2disk controller i/o error,error=%x\n",error);
    }
    slr.process_switch (irq14_flag);
  }
  else 
  {
    inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);unsigned char status = inportb (0x1f7);
  }
}

extern "C" void secondarydiskhandler()
{
  outportb(0xA0, 0x20);
  outportb(0x20, 0x20);

  if (irq15_flag == -2)
  {
    irq15_flag = -1;
  }
  else if (round_robin_start == 0)
  {
    unsigned char status = inportb (0x177);

    if ( status & 0x1 )
    {
      unsigned char error = inportb (0x171);
      irq15_flag = (unsigned int)error & 0xff;
      disp1.printf ("2disk controller i/o error,error=%x\n",error);
    }
 
    else if (((status & 0x80) == 0) && ((status & 0x08) != 0))
      irq15_flag=1;       
  }
  else if (round_robin_start == 1 && irq15_flag != -1)  
  {
    inportb (0x177);inportb (0x177);inportb (0x177);inportb (0x177);unsigned char status = inportb (0x177);

    if ( status & 0x1 )
    {
      unsigned char error = inportb (0x171);
      irq15_flag = (unsigned int)error & 0xff;
      disp1.printf ("2disk controller i/o error,error=%x\n",error);
    }
    slr.process_switch (irq15_flag);
  }
  else 
  {
    inportb (0x177);inportb (0x177);inportb (0x177);inportb (0x177);unsigned char status = inportb (0x177);
  }
}


extern "C" void floppy_interrupt()
{
	irq6_flag=1;
	outportb(0x20, 0x20);
}

extern "C" void syscall_handler (struct reg_val *r)
{
  if(r->eax == 1)
  {
    printfhandler(r);
  }
  else if(r->eax==2)
  {
    irq1_flag=slr.return_current_active_pid();
    scanfhandler(r);
    irq1_flag=-1;
  }
  else if (r->eax ==3)
  {
    memoryhandler (r);
  }
  else if (r->eax ==4)
  {
    filesystemhandler (r);
  }
  else if (r->eax==5)
  {
    processhandler (r);
  }
  else if (r->eax==6)
  {
    stringhandler (r);
  }
  else if (r->eax==7)
  {
    machine_functions (r);
  }
}
	

void idt_setup ()
{
  int i;

  fill_idt_entry (idt_entries[0],(unsigned int)&exception0);
  fill_idt_entry (idt_entries[1],(unsigned int)&exception1);
  fill_idt_entry (idt_entries[2],(unsigned int)&exception2);
  fill_idt_entry (idt_entries[3],(unsigned int)&exception3);
  fill_idt_entry (idt_entries[4],(unsigned int)&exception4);
  fill_idt_entry (idt_entries[5],(unsigned int)&exception5);
  fill_idt_entry (idt_entries[6],(unsigned int)&exception6);
  fill_idt_entry (idt_entries[7],(unsigned int)&exception7);
  fill_idt_entry (idt_entries[8],(unsigned int)&exception8);
  fill_idt_entry (idt_entries[9],(unsigned int)&exception9);
  fill_idt_entry (idt_entries[10],(unsigned int)&exception10);
  fill_idt_entry (idt_entries[11],(unsigned int)&exception11);
  fill_idt_entry (idt_entries[12],(unsigned int)&exception12);
  fill_idt_entry (idt_entries[13],(unsigned int)&exception13);
  fill_idt_entry (idt_entries[14],(unsigned int)&exception14);
  fill_idt_entry (idt_entries[15],(unsigned int)&exception15);
  fill_idt_entry (idt_entries[16],(unsigned int)&exception16);
  fill_idt_entry (idt_entries[17],(unsigned int)&exception17);
  fill_idt_entry (idt_entries[18],(unsigned int)&exception18);
  
  for(i=19;i<32;i++)
    fill_idt_entry (idt_entries[i],(unsigned int)&exception19);

  for (i=35;i<48;i++)
    fill_idt_entry (idt_entries[i],(unsigned int)&timerisr);

  fill_idt_entry (idt_entries[32],(unsigned int)&timerisr);
  fill_idt_entry (idt_entries[33],(unsigned int)&keyboardisr);
  fill_idt_entry (idt_entries[46],(unsigned int)&primarydiskisr);
  fill_idt_entry (idt_entries[47],(unsigned int)&secondarydiskisr);
  fill_idt_entry (idt_entries[38],(unsigned int)&floppyisr);

  fill_idt_entry (idt_entries[99],(unsigned int)&syscallisr); 
 	
  iptr.base = idt_entries;
  iptr.limit = 0x7ff;

}

void irqremap()
{
  /* ICW1 - port 0x20 is master pic and port 0xa0 is slave pic */
  
  outportb(0x20,0x11);
  outportb(0xA0,0x11);

  /* ICW2 - port 0x21 is master pic and port 0xa1 is slave pic 
     remapping is done here */
  outportb(0x21,0x20);
  outportb(0xA1,0x28);

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
