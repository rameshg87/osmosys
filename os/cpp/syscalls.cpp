/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/

/*****************************************

SYSCALLS LIST

EAX=1	PRINT FUNCTIONS
	EBX=1	PUTCHAR
	EBX=2	PRINTF
	EBX=3	GOTOXY
	EBX=4	CLEAR SCREEN
	EBX=5	GETY
	EBX=6	SETATTRIB

EAX=2	KEYBOARD FUNCTIONS
	EBX=1	READ CHARACTER
	EBX=2	READ DECIMAL INT
	EBX=3	READ STRING

EAX=3	MEMORY FUNCTIONS
	EBX=1	MALLOC
	EBX=2	FREE

EAX=4	FILE FUNCTIONS
	EBX=1	OPEN
	EBX=2	READ
	EBX=3	WRITE
	EBX=5	SEEK
	EBX=4	CLOSE
	EBX=6	CREATE FILE
	EBX=7	DELETE
	EBX=8	CREATE DIRECTORY
	EBX=9	LIST
	EBX=10	CHANGE DIRECTORY
	EBX=11	FILE SEARCH

EAX=5	PROCESS FUNCTIONS
	
*****************************************/


#include <idt.h>
#include <video.h>
#include <syscalls.h>
#include <process.h>

extern display disp1;
extern bool irq1_status;

extern scheduler slr;
extern logical_memory klm;
extern logical_memory *current_active_memory;
extern filemanager fm;
extern int current_pid;
extern char scancode;
extern int flagger;
extern int irq1_flag;
extern int irq2_flag;
extern char *cd_name;
extern scancode_list *schead;
bool caps=0;

extern "C" void systemcallbackup ();

char scancode_lookup1[128]={
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,	/* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`',   0,		/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',   0,	/* Right shift */
    '*',
    0,	/* Alt */
    ' ',/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    145,	/* Home key */
    140,	/* Up Arrow */
    146,	/* Page Up */
    '-',
    141,	/* Left Arrow */
    0,
    142,	/* Right Arrow */
    '+',
    148,	/* 79 - End key*/
    143,	/* Down Arrow */
    149,	/* Page Down */
    144,	/* Insert Key */
    147,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

char scancode_lookup2[128]=
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b',
    '\t',
    'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,	/* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '\'', '~',   0,		/* Left shift */
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?',   0,	/* Right shift */
    '*',
    0,	/* Alt */
    ' ',/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    145,	/* Home key */
    140,	/* Up Arrow */
    146,	/* Page Up */
    '-',
    141,	/* Left Arrow */
    0,
    142,	/* Right Arrow */
    '+',
    148,	/* 79 - End key*/
    143,	/* Down Arrow */
    149,	/* Page Down */
    144,	/* Insert Key */
    147,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
char scancode_lookup3[128]={
    0,  27, 162, 163, 164, 165, 166, 167, 168, 169,
    170,171, '-', '=', '\b',
    172,
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', 173,	/* Enter key */
    0,	/* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`',   0,		/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',   0,	/* Right shift */
    '*',
    0,	/* Alt */
    ' ',/* Space bar */
    0,	/* Caps lock */
    150,	/* 59 - F1 key ... > */
    151,   152,   153,   154,   155,   156,   157,   158,
    159,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    145,	/* Home key */
    140,	/* Up Arrow */
    146,	/* Page Up */
    '-',
    141,	/* Left Arrow */
    0,
    142,	/* Right Arrow */
    '+',
    148,	/* 79 - End key*/
    143,	/* Down Arrow */
    149,	/* Page Down */
    144,	/* Insert Key */
    147,	/* Delete Key */
    0,   0,   0,
    160,	/* F11 Key */
    161,	/* F12 Key */
    0,	/* All other keys are undefined */
};
char scancode_lookup4[128]={
    0,  27, 186, 187, 188, 189, 190, 191, 192, 193,
    194, 195, '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,	/* Control */
    'a', 196, 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`',   0,		/* Left shift */
    '\\', 197, 200, 199, 201, 'b', 'n',
    'm', ',', '.', '/',   0,	/* Right shift */
    '*',
    0,	/* Alt */
    198,/* Space bar */
    0,	/* Caps lock */
    174,	/* 59 - F1 key ... > */
    175,   176,   177,   178,   179,   180,   181,   182,
    183,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    145,	/* Home key */
    140,	/* Up Arrow */
    146,	/* Page Up */
    '-',
    141,	/* Left Arrow */
    0,
    142,	/* Right Arrow */
    '+',
    148,	/* 79 - End key*/
    143,	/* Down Arrow */
    149,	/* Page Down */
    144,	/* Insert Key */
    147,	/* Delete Key */
    0,   0,   0,
    184,	/* F11 Key */
    185,	/* F12 Key */
    0,	/* All other keys are undefined */
};


void printfhandler(struct reg_val *r)
{
	switch (r->ebx)
	{
		case 1:
			{
			unsigned int *arg = (unsigned int *)r->user_esp;
			char temp = (*arg) & 0xff;
			disp1.putchar (temp);
			break;
			}
		case 2:	
			{
			void *cstring;
			unsigned char *arg;
			unsigned int *par;
			char count;
			cstring=(void *)(*((unsigned int *)r->user_esp));
	
		

			count=0;
			arg=(unsigned char *)cstring;
			par=(unsigned int *)r->user_esp + 1;
	
			while(*arg!='\0')
			{
				if((*arg=='%')&&(*(arg+1)=='x'))
				{
					int temp=*par;
					disp1.putint_hexadecimal(temp);
					arg+=1;
					par+=1;
				}
				else if((*arg=='%')&&(*(arg+1)=='c'))
				{
					char temp=(char)*par;
					disp1.putchar(temp);
					arg+=1;
					par+=1;
				}
				else if((*arg=='%')&&(*(arg+1)=='s'))
				{
					char *temp=(char *)((unsigned int)(*par));
					disp1.putstring(temp);
					arg+=1;
					par+=1;
				}
				else if((*arg=='%')&&(*(arg+1)=='d'))
				{
					int temp=*par;
					disp1.putint_decimal(temp);
					arg+=1;
					par+=1;
				}
				else
					disp1.putchar(*arg);
				arg+=1;
			}
			break;
			}
		case 3:
			{
			unsigned int *xpos = (unsigned int *)r->user_esp;			
			unsigned int *ypos = xpos +1;
			disp1.gotoxy (*xpos,*ypos);
			break;
			}
		case 4:
			disp1.clear_screen ();
			break;
		case 5:
			r -> eax = disp1.gety ();
			break;
		case 6:
			unsigned int *attrib = (unsigned int *)r->user_esp;			
			disp1.setattrib ((char)*attrib);
			break;

	}
}		

void memoryhandler (struct reg_val *r)
{
	switch (r->ebx)
	{
		case 1:
			{
			unsigned int *mem_req = (unsigned int *)r->user_esp;
			void *addr = current_active_memory -> kdmalloc (*mem_req);
			r->eax = (unsigned int)addr;
			break;
			}
		case 2:
			{
			unsigned int *mem_alloc = (unsigned int *)r->user_esp;
			unsigned int *mem_addr = mem_alloc ++;
			current_active_memory -> kdfree ((void*)*mem_addr,*mem_alloc);
			break;
			}
	}
}

void filesystemhandler (struct reg_val *r)
{
	switch (r->ebx)
	{
		case 1:
			{
			char *fname = (char*)(*((unsigned int *)r->user_esp));
			char *mode = (char*)(*((unsigned int *)r->user_esp+1));
			r -> eax = fm.open (fname,mode);
			break;
			}
		case 2:
			{
			unsigned char *tmpbuf;
			unsigned int *fid = (unsigned int *)r->user_esp;
			unsigned int *buffer = (unsigned int*)(*((unsigned int *)r->user_esp+1));
			unsigned int *count = (unsigned int *)r->user_esp+2;
			r->eax = fm.read (*fid,&tmpbuf,*count);
			*buffer = (unsigned int)tmpbuf;
			break;
			}
		case 3:
			{
			unsigned int *fid = (unsigned int*)r->user_esp;
			unsigned char *buffer = (unsigned char*)(*((unsigned int *)r->user_esp+1));
			unsigned int *count = (unsigned int *)r->user_esp+2;
			r -> eax = fm.write (*fid,buffer,*count);
			break;
			}
		case 4:
			{
			/* dont know why, useresp is lost in fclose , hence backing it up */
			unsigned int useresp = (unsigned int)r->user_esp;
			unsigned int *fid = (unsigned int*)r->user_esp;
			fm.close (*fid);
			r->user_esp = useresp;
			break;
			}
		case 5:
			{
			unsigned int *fid = (unsigned int *)r->user_esp;
			unsigned int *count = (unsigned int *)r->user_esp+1;						
			unsigned int *from = (unsigned int *)r->user_esp+2;									
			fm.seek (*fid,*count,*from);
			break;
			}
		case 9:
			{
			char *kstr;
			kstr=(char *)klm.kdmalloc(strlen(cd_name)+2);
			strcpy(kstr,cd_name);
			//disp1.printf("str = %s",kstr);
			if (strlen(kstr) == 2)
			{
				kstr[strlen(kstr)+1] = '\0';
				kstr[strlen(kstr)] = '/';
			}
			fm.list(kstr);
			break;
			}
		case 10:
			{
			char *fname = (char *)(*((unsigned int *)r->user_esp));
			char *temp;
			bool found = 0;
			temp = (char *)klm.kdmalloc(strlen(fname)+1);
			strcpy(temp,fname);
			found = fm.file_search(temp,0);
			if(found) cd_name=temp;
			else disp1.printf("INVALID DIRECTORY\n");

			//fm.cd (temp);
			char *string;
			string=(char *)slr.running_list_element->ptr->memobj->kdmalloc(strlen(cd_name)+1);
			strcpy(string,cd_name);
			r->eax = (int)string;
			break;
			}
		case 11:
			{
			char *fname = (char *)(*((unsigned int *)r->user_esp));
			unsigned char file = (unsigned char)(*((unsigned int *)r->user_esp+1));
			//char *kname = (char *)klm.kdmalloc(strlen(fname)+1);
			//unsigned char kfile = file;
			//strcpy(kname,fname);
			r -> eax = (int) fm.file_search(fname,file);
			break;
			}
		case 6:
			{
			char *fname = (char*)(*((unsigned int *)r->user_esp));
			r -> eax = (int)fm.create_file (fname);
			break;
			}
		case 7:
			{
			char *fname = (char*)(*((unsigned int *)r->user_esp));
			r -> eax = (int)fm.delete_file (fname);
			break;
			}
		case 8:
			{
			char *fname = (char*)(*((unsigned int *)r->user_esp));
			r -> eax = (int)fm.create_dir (fname);
			break;
			}
	}
}

void machine_functions (struct reg_val *r)
{
	switch (r -> ebx)
	{
		case 1:	//Shutdown
			outportw (0xb004,0x2000);
			break;

		case 2: //Reboot
			{		
			unsigned char bad = 0x02;
			while ((bad & 0x02) != 0)
			{
				bad = inportb (0x64);
			}
			outportb (0x64,0xFE);
			break;
			}
		case 3:
			sysinfo ();
			break;
	}

}
		
void scanfhandler(struct reg_val *r)
{
   bool shift=0,alt=0,ctrl=0;
   char count,*string;
   unsigned char press;
   int i; 

    count=1;
    string = (char*) slr.running_list_element -> ptr -> memobj -> kdmalloc (200);
    if(r->err_code==0)count=0;
    r->err_code=1;

    do 
    {
      flagger = 1;
      irq1_status=0;
	schead = 0;
      systemcallbackup ();

    scancode_list *temp = schead;

    while (temp != 0)
    {
	unsigned char scancode = temp -> scancode;
	
    if(!(scancode&0x80))/* key press */
    {
      if((scancode==LSHIFT)||(scancode==RSHIFT))
      {
	       shift=1;
	      temp=temp->next;
	continue;
      }
      else if(scancode==CAPS)
      {
		caps=~caps;
	      temp=temp->next;
	continue;
	}
      else if(scancode==BACKSPACE)
      {
	  if(count>0)
	  {
	    count--;
	    unsigned int x = disp1.getx();
	    unsigned int y = disp1.gety();
	    disp1.gotoxy(x-1,y);
	    disp1.putchar(' ');
	    disp1.gotoxy(x-1,y);
	  }
	  temp=temp->next;
	  continue;
 	}
	else if(scancode==ALT)
       {
		alt=1;
	       temp=temp->next;
	continue;
	}
	else if(scancode==CTRL)
      {
		ctrl=1;
	      temp=temp->next;
	continue;
	}
      else if((scancode==ENTER)&&(r->ebx==1))
      {
	string[count]='\0';
	i=strtoi(string);
	r->eax=i;
	return;
      }
      else if((scancode==ENTER)&&(r->ebx==2))
      {
	string[count]='\0';
	r->eax=((unsigned int)string);
	return;
      }
      else
      {
        if(shift==1)
	{
	  press=scancode_lookup2[scancode];
	  if(isalpha(press)&&caps)
	   press+=32;
	}
	else if(alt==1)
	{
		press=scancode_lookup3[scancode];
		if(ctrl==1 && press==147)
		{
			press=207;
		}
			}
	else if(ctrl==1)
	{
		press=scancode_lookup4[scancode];
	}
	else
	{
		  press=scancode_lookup1[scancode];
	  if(isalpha(press)&&caps)
	   press-=32;
	}
      }
    }
    else
    {
	
      char temp1=(LSHIFT|0x80);
      char temp2=(RSHIFT|0x80);
      if((scancode==0xAA)||(scancode==0xB6))
       shift=0;
      else if(scancode==0xB8)
	      alt=0;
      else if(scancode==0x9D)
	      ctrl=0;
    }

    switch(r->ebx)
    {
      case 0:	disp1.printf("%c",press); 
		r -> eax = press & 0xFF;
		return;
      break;
      case 1:if(!(scancode&0x80))
	     {	      
	      if(isnum(press) || (press == '-' && count==0))
		{
		  disp1.printf("%c",press);
		  string[count]=press;
		  count++;
		}
	     }
	//     scanfhandler(r);
      break;
      case 2:if(!(scancode&0x80))
	     {	      
	        disp1.printf("%c",press);
		string[count]=press;
		count++;
	     }
	  //   scanfhandler(r);
      break; 
    }
    temp = temp -> next;
    }

    }while (1);
}


void processhandler (struct reg_val *r)
{

  unsigned int pid,ppid,val;
  process *p3;
  
  switch(r->ebx)
  {
    case 1:
	{
	  char *name = (char *)(*((unsigned int *)r->user_esp));
	   p3=new process(name);
	    break;
	}
    case 2:
	{
	  char *name = (char *)(*((unsigned int *)r->user_esp));
	   p3=new process(name);
	   p3->set_ppid(current_pid);
	   //disp1.printf("%d ",current_pid);
	   irq2_flag=current_pid;
	   r->eax=p3->get_pid();
	   flagger=2;

	   systemcallbackup();
	   irq2_flag=-1;
	    break;
	}
    case 3:
	{
	  char *name = (char *)(*((unsigned int *)r->user_esp));
	   process *temp;
	   pid=(unsigned int)name;

	   if(current_pid==pid)
	   {
	     temp=slr.running_list_element->ptr;
	     ppid=temp->get_ppid();
	     //slr.process_switch(pid);
	   }
	   /* process is in the ready queue 
	   else
	   {
	   }*/

	   //klm.kdfree(temp->memobj,sizeof(logical_memory));
	   //klm.kdfree(temp,sizeof(process));

	   //disp1.printf("%d %d",current_pid,ppid);
	   //slr.remove_from_waiting_list(pid);
	   slr.process_switch(ppid);
	
    break;
	}
    case 4:
	   r->eax=current_pid;
    break;
  }
}

void stringhandler (struct reg_val *r)
{
	switch (r->ebx)
	{
		case 1:
			{
			char *sname = (char*)(*((unsigned int *)r->user_esp));
			int length = strlen (sname);
			r->eax = length;
			break;
			}
		case 2:
			{
			char *sname1 = (char*)(*((unsigned int *)r->user_esp));
			char *sname2 = (char*)(*((unsigned int *)r->user_esp+1));
			strcpy (sname1,sname2);
			break;
			}
		case 3:
			{
			char *sname1 = (char*)(*((unsigned int *)r->user_esp));
			char *sname2 = (char*)(*((unsigned int *)r->user_esp+1));
			strcat (sname1,sname2);
			break;
			}
		case 4:
			{
			char *sname1 = (char*)(*((unsigned int *)r->user_esp));
			char *sname2 = (char*)(*((unsigned int *)r->user_esp+1));
			int match = strcmp (sname1,sname2);
			r->eax = match;
			break;
			}
	}
}
