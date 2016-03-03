/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#include <filesystem.h>
#include <video.h>
#include <idt.h>
#include <memory.h>
#include <common.h>
extern int irq14_flag;
extern int irq15_flag;
extern logical_memory klm; 
extern display disp1; 
extern int irq6_flag;
extern int current_pid;
extern int flagger;
extern "C" void systemcallbackup ();
extern char round_robin_start;

void primarydisk::piowrite (int lba_address,int count,unsigned char* diskbuffer)
{
  while (irq14_flag != -1);
 
  int idx;
  unsigned short int tmpword;

  /* tell controller that we are going to give lba28 address */
  outportb (0x1F1,0x00);

  outportb (0x1F2,count);

  /* lba address bits 7-0 */
  outportb (0x1F3,(lba_address)&0xFF);

  /* lba address bits 15-8 */
  outportb (0x1F4,(lba_address>>8)&0xFF);

  /* lba address bits 23-16 */
  outportb (0x1F5,(lba_address>>16)&0xFF);

  /* magic bit + drive number (master / slave) + lba address bits 27-24 */
  outportb (0x1F6,0xE0 | (dtype) | ((lba_address>>24) & 0x0F));

  /* write command */
  outportb (0x1F7, 0x30);

  /* write the sector word by word which is stored in disk buffer */
  for (idx = 0; idx < 256*count; idx++)
  {
    tmpword = diskbuffer[ idx * 2] | (diskbuffer[idx * 2 + 1] << 8);
    outportw(0x1F0, tmpword);
    if (idx % 256 == 0)
    {
	unsigned char status;
	inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);
      do{status = inportb (0x1f7);}
      while (status & 0x08 == 0);  
    }
  }
 
  if (round_robin_start == 0)
  {
    while (irq14_flag != 1);  
  }
  else
  {
    flagger=14;
    systemcallbackup ();
  }
  irq14_flag = -1;
}


/* this function reads a sector given the lba address and disk buffer index
   to which the sector is to be loaded */

void primarydisk::pioread(int lba_address,int count,unsigned char *diskbuffer)
{
  while (irq14_flag != -1);
  int idx;
  unsigned short int tmpword;

  /* tell controller that we are going to give lba28 address */
  outportb (0x1F1,0x00);

  outportb (0x1F2,count);

  outportb (0x1F3,(lba_address)&0xFF);
  /* lba address bits 7-0 */

  /* lba address bits 15-8 */
  outportb (0x1F4,(lba_address>>8)&0xFF);

  /* lba address bits 23-16 */
  outportb (0x1F5,(lba_address>>16)&0xFF);

  /* magic bit + drive number (master / slave) + lba address bits 27-24 */
  outportb (0x1F6,0xE0 | (dtype) | ((lba_address>>24) & 0x0F));

  /* read command */

  if (round_robin_start == 0)
  {
    irq14_flag=0;
    outportb (0x1F7, 0x20);
    /* wait for the controller to get ready */
    while (!irq14_flag);
  }
  else
  {
	irq14_flag = current_pid;
	flagger = 14;
        outportb (0x1F7, 0x20);
	systemcallbackup ();
  }

  /* read the sector word by word and store it in disk buffer */
  for (idx = 0; idx < 256*count; idx++)
  {
	if (idx %256 == 0)for (int ui=0;ui<0xfff;ui++);
        tmpword = inportw(0x1F0);
        diskbuffer [idx*2] = (unsigned char)(tmpword & 0xFF);
        diskbuffer [idx*2+1] = (unsigned char)((tmpword>>8) & 0xFF);
  }
  irq14_flag = -1;
}

primarydisk::primarydisk(unsigned char dt,int dno) 
{
	dtype = dt;
	driveno = dno;
}

secondarydisk::secondarydisk(unsigned char dt,int dno) 
{
	dtype = dt;
	driveno = dno;
}


drive::drive()
{

}

void drive::pioread(int i,int j,unsigned char* diskbuffer)
{

}

void drive::dmaread(int i,int j,unsigned char* diskbuffer)
{

}


void drive::piowrite(int i,int j,unsigned char* diskbuffer)
{

}

void secondarydisk::piowrite (int lba_address,int count,unsigned char* diskbuffer)
{
  int idx;
  unsigned short int tmpword;

  /* tell controller that we are going to give lba28 address */
  outportb (0x171,0x00);

  outportb (0x172,count);

  /* lba address bits 7-0 */
  outportb (0x173,(lba_address)&0xFF);

  /* lba address bits 15-8 */
  outportb (0x174,(lba_address>>8)&0xFF);

  /* lba address bits 23-16 */
  outportb (0x175,(lba_address>>16)&0xFF);

  /* magic bit + drive number (master / slave) + lba address bits 27-24 */
  outportb (0x176,0xE0 | (dtype) | ((lba_address>>24) & 0x0F));

  
  /*write command*/ 
  outportb (0x177, 0x30);

  irq15_flag = 100;
  /*write the sector word by word which is stored in disk buffer */


  for (idx = 0; idx < 256*count; idx++)
  {
    tmpword = diskbuffer[ idx * 2] | (diskbuffer[idx * 2 + 1] << 8);
    outportw(0x170, tmpword);
    if (idx % 256 == 0)
    {
      unsigned char status = inportb (0x177);
      while (status & 0x08 == 0);  
    }

  }

  /* wait for the controller to get ready */
  while (irq15_flag!=1);
}


/* this function reads a sector given the lba address and disk buffer index
   to which the sector is to be loaded */

void secondarydisk::pioread(int lba_address,int count,unsigned char *diskbuffer)
{

  while (irq15_flag != -1);
  int idx;
  unsigned short int tmpword;

  /* tell controller that we are going to give lba28 address */
  outportb (0x171,0x00);

  outportb (0x172,count);

  outportb (0x173,(lba_address)&0xFF);
  /* lba address bits 7-0 */

  /* lba address bits 15-8 */
  outportb (0x174,(lba_address>>8)&0xFF);

  /* lba address bits 23-16 */
  outportb (0x175,(lba_address>>16)&0xFF);

  /* magic bit + drive number (master / slave) + lba address bits 27-24 */
  outportb (0x176,0xE0 | (dtype) | ((lba_address>>24) & 0x0F));

  /* read command */

  if (round_robin_start == 0)
  {
    irq15_flag=0;
    outportb (0x177, 0x20);
    /* wait for the controller to get ready */
    while (!irq15_flag);
  }
  else
  {
	irq15_flag = current_pid;
	flagger = 15;
        outportb (0x177, 0x20);
	systemcallbackup ();
  }

  /* read the sector word by word and store it in disk buffer */
  for (idx = 0; idx < 256*count; idx++)
  {
	if (idx %256 == 0)for (int ui=0;ui<0xfff;ui++);
        tmpword = inportw(0x170);
        diskbuffer [idx*2] = (unsigned char)(tmpword & 0xFF);
        diskbuffer [idx*2+1] = (unsigned char)((tmpword>>8) & 0xFF);
  }
  irq15_flag = -1;

}


void floppy_drive::dma_access(int lba_address,int count,int mode)
{
	int mrq,index,number_of_sectors_per_track=18,number_of_tracks=80,number_of_heads=2,cylinder,head,sector,tcount;

	unsigned char st1,st2,result_byte,byte_per_sector; 
	unsigned char opcode[9],st0,cyl,mod,fcmd;
	unsigned short int hcount;
	/*
	char status[4][20] =
            {" 0", "error", "invalid", "drive" };
	*/
	sector = (lba_address%number_of_sectors_per_track)+1;   //Convert from lba address to CHS
	cylinder = (lba_address/number_of_sectors_per_track)/number_of_heads;
	head = (lba_address/number_of_sectors_per_track)%number_of_heads;
	
	tcount=count*512;
		
	if(tcount>65535)
	{
		disp1.printf("Count Error........\nRead/Write is allowed for 0 to 127 sectors at a time");
	for(;;);		
	}
	hcount=(unsigned short int)tcount;
//	disp1.printf("Count is %x\n",hcount);
	if(mode==0)
	{
	   mod=0x46;              ///for read
	   fcmd=0xC6;
	}

       			
	else if(mode==1)
	{
	 mod=0x4A;      ///for write
	 fcmd=0xC5;
	
	}			
	else
	{
	disp1.printf("Undefined mode(0--->read,1-->write)");
		for(;;);
	}
	
	//enabling drive...............................
	
	
	
	outportb(0x3F2,0x00);
	outportb(0x3F2,0x0C);
	while(!irq6_flag);
	irq6_flag=0;
	
	
		
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x08);
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  st0=inportb(0x3F5);
                  break;
		}
	}
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  cyl=inportb(0x3F5);
                  break;
		}
	}
	//floppy reset.........................
	outportb(0x3F7,0x00);
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x03);
                  break;
		}
	}
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0xDF);
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x02);
                  break;
		}
	}
	//recalibration........................
	outportb(0x3F2,0x1C);
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x07);
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x00);
                  break;
		}
	}
	
	
	while(!irq6_flag);
	irq6_flag=0;
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x08);
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  st0=inportb(0x3F5);
                  break;
		}
	}
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  cyl=inportb(0x3F5);
                  break;
		}
	}
	if(!cyl)
	{
		outportb(0x3F2,0x00);
	
//	disp1.printf("caliberation success.....\n");
	
	}


	outportb(0x3F2,0x1C);
	
	while(!irq6_flag);
	irq6_flag=0;
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x08);
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  st0=inportb(0x3F5);
                  break;
		}
	}
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  cyl=inportb(0x3F5);
                  break;
		}
	}
		
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x0F);
                  break;
		}
	}

	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x00);
                  break;
		}
	}
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x00);
                  break;
		}
	}

	
	while(!irq6_flag);
	irq6_flag=0;

	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x08);
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  st0=inportb(0x3F5);
                  break;
		}
	}
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  cyl=inportb(0x3F5);
                  break;
		}
	}
		

	if(cyl==0x00)
	{
		outportb(0x3F2,0x00);
	
///	disp1.printf("cylinder in right position..\n");
	
	}
		
	
	outportb(0x3F2,0x1C);
	
	
	
	///////////////DMA initialization.....
	unsigned char dma_buffer[0x4800] __attribute__((aligned(0x10000)));
	union {
			unsigned char b[4];
			unsigned long l;
		}a,c;
	a.l=0x500000;
	c.l=hcount-1;
	if((a.l >> 24) || (c.l >> 16) || (((a.l&0xffff)+c.l)>>16))
	{		
		disp1.printf("floppy_dma_init: static buffer problem\n");
	for(;;);
	}
	outportb(0x0A,0x06);
	outportb(0x0C,0xFF);
	outportb(0x04, a.b[0]); //  - address low byte
	outportb(0x04, a.b[1]); //  - address high byte
	outportb(0x81, a.b[2]); // external page register
	outportb(0x0c, 0xff);   // reset flip-flop
	outportb(0x05, c.b[0]); //  - count low byte
	outportb(0x05, c.b[1]); //  - count high byte
	outportb(0x0b, mod);   // set mode (see above)
	outportb(0x0a, 0x02);   // unmask chan 2
//////ends................................


	/*disp1.printf("cylinder  %x\n",((unsigned char )(0xff & cylinder)));
	disp1.printf("sector  %x\n",((unsigned char )(0xff & sector)));
	disp1.printf("head %x\n",((unsigned char )(0xff & head))<<2);
*/	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,fcmd);    /////////
                  break;
		}
	}
		
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,(unsigned char )((0xff & head)<<2));      //////head and drive
                  break;
		}
	}

	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,((unsigned char )(0xff & cylinder)));       /////cylinder
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,((unsigned char )(0xff & head)));    ///////////// head
		  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,((unsigned char )(0xff & sector)));      ////// sector
                  break;
		}
	}

	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x02);    //bytes /sector
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x12);   ///18sectors per track
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0x2A);  ///GAP 3
                  break;
		}
	}

//for(;;);
	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  outportb(0x3F5,0xFF);   /////data length
                  break;
		}
	}

	
	while(!irq6_flag);
	irq6_flag=0;


	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  st0=inportb(0x3F5); 
                  break;
		}
	}

	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  st1=inportb(0x3F5); 
                  break;
		}
	}	
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  st2=inportb(0x3F5); 
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  cylinder=inportb(0x3F5); 
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  head=inportb(0x3F5); 
                  break;
		}
	}
	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  sector=inportb(0x3F5); 
                  break;
		}
	}

	while(1)
	{
		if((inportb(0x3F4) & 0x80))
		{
                  byte_per_sector=inportb(0x3F5); 
                  break;
		}
	}
	outportb(0x3F2,0x00);

	
	//disp1.printf("Got interrupt..\n");

	int error=0;
	if(st1 & 0x80) {
    	disp1.printf("floppy_do_sector: end of cylinder\n");
    	error = 1;
	for(;;);
	}
	if(st0 & 0x08) {
    	disp1.printf("floppy_do_sector: drive not ready\n");
    	error = 1;
	for(;;);
	}
	if(st1 & 0x20) {
    	disp1.printf("floppy_do_sector: CRC error\n");
    	error = 1;
	for(;;);
	}
	if(st1 & 0x10) {
    	disp1.printf("floppy_do_sector: controller timeout\n");
    	error = 1;
	for(;;);
	}
	if(st1 & 0x04) {
    	disp1.printf("floppy_do_sector: no data found\n");
    	error = 1;
	for(;;);
	}
	if((st1|st2) & 0x01) {
            disp1.printf("floppy_do_sector: no address mark found\n");
            error = 1;
	for(;;);
        }
        if(st2 & 0x40) {
            disp1.printf("floppy_do_sector: deleted address mark\n");
            error = 1;
	for(;;);
        }
        if(st2 & 0x20) {
            disp1.printf("floppy_do_sector: CRC error in data\n");
            error = 1;
	for(;;);
        }
        if(st2 & 0x10) {
            disp1.printf("floppy_do_sector: wrong cylinder\n");
            error = 1;
	for(;;);
        }
        if(st2 & 0x04) {
            disp1.printf("floppy_do_sector: uPD765 sector not found\n");
            error = 1;
	for(;;);
        }
        if(st2 & 0x02) {
            disp1.printf("floppy_do_sector: bad cylinder\n");
            error = 1;
	for(;;);
        }
        if(byte_per_sector != 0x2) {
            disp1.printf("floppy_do_sector: wanted 512B/sector, got %d",(1<<(byte_per_sector+7)));
            error = 1;
	for(;;);
        }
        if(st1 & 0x02) {
            disp1.printf("floppy_do_sector: not writable\n");
            error = 2;
	for(;;);
        }
        if(!error) {
	 disp1.printf("Floppy Operation Success\n");
          
       }
        if(error >= 1) {
            disp1.printf("Error occured while floppy operation\n");
	for(;;);

       }

	
}
	
	
floppy_drive::floppy_drive()
{
	

// Obviously you'd have this return the data, start drivers or something.
/*
	char drive_types[8][40] = {
	"none",
	"360kB 5.25\"",
	"1.2MB 5.25\"",
	"720kB 3.5\"",
	"1.44MB 3.5\"",
	"2.88MB 3.5\"",
	"unknown type",
	"unknown type"
	};
*/
   outportb(0x70, 0x10);
   unsigned drives = inportb(0x71);

  if (drives  == 64)
     disp1.printf("\n1.44MB Floppy Drive found\n");
  else if(drives  == 4)
     disp1.printf("\nNo device connected\n");
  else 
  disp1.printf("\nUnknown device type\n");
}
	
