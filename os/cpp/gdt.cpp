/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#include <gdt.h>
#include <video.h>

extern tss_entry_t tss;
extern display disp1;
extern gdt_ptr gptr;
extern gdt_entry segment[1024];

extern "C" void load_gdt(unsigned int);
extern "C" void load_tss ();


gdt_entry gdt_assign(unsigned int limit,unsigned int base,unsigned char access,unsigned char granularity)
{
	gdt_entry temp;
	temp.limit_low=limit&0xffff;
	temp.base_low=base&0xffff;
	temp.base_middle=(base>>16)&0xff;
	temp.access=access;
	temp.granularity=(0xf&limit>>16)|(granularity<<4);
	temp.base_high=(base>>24)&0xff;
	return temp;
}

void gdt_setup()
{
  segment[0]=gdt_assign(0,0,0,0);
  segment[1]=gdt_assign(0xfffff,0,0x9a,0xc);
  segment[2]=gdt_assign(0xfffff,0,0x92,0xc);
  segment [3] = gdt_assign (0xfffff,0x0,0xfa,0xc);               //dummy user code segment
  segment [4] = gdt_assign (0xfffff,0x0,0xf2,0xc);   //dummy user data segment
  segment [5] = gdt_assign (sizeof (tss_entry_t),(unsigned int)&tss,0xe9,0x0);    //tss descriptor
  segment [6] = gdt_assign (0xffffffff,0x0,0xf2,0xc);    //stack segment for user processes.
  gptr.base=(unsigned int)(segment);
  gptr.limit=55;
  load_gdt((unsigned int)(&gptr));
	//giving tss entries to the  tss strusture............................
  tss.esp0 = 0x9ffff;
  tss.ss0  = 0x10;
  tss.cs = 0x0b;
  tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
	//loading tss......tr register
  load_tss ();
}

