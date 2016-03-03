/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#include <common.h>
#include <video.h>
#include <memory.h>
#include <filemanager.h>
#include <idt.h>

extern display disp1;
extern physical_memory pm; 
extern disk_info *dih,*dit;
extern long sys_clock;

//function to out a byte to the specified port
void outportb (unsigned short int port,unsigned char data)
{
  /* put port address in dx, put data byte in os */
  __asm__ ("out %%al, %%dx" : : "a" (data) , "d" (port));
}

unsigned char inportb (unsigned short int port)
{
  /* put port address in dx, read result from accumulator */

  unsigned char result;
  __asm__ ("in %%dx,%%al" : "=a" (result) : "d" (port));
  return result;
}

//function to out a word to the specified port
void outportw (unsigned short int port,unsigned short int data)
{
  /* put port address in dx, put data byte in os */

  __asm__ ("out %%ax, %%dx" : : "a" (data) , "d" (port));
}

unsigned short int inportw (unsigned short int port)
{
  /* put port address in dx, read result from accumulator */

  unsigned short int result;
  __asm__ ("in %%dx,%%ax" : "=a" (result) : "d" (port));
  return result;
}

unsigned int return_int (unsigned char *buffer,int offset,bool littleendian)
{
  int dword;
  if (!littleendian)
  {
    dword = buffer[offset];
    dword = dword & 0xFF;
    dword = dword | (buffer[offset+1]<<8);
    dword = dword & 0xFFFF;	  
    dword = dword | (buffer[offset+2]<<16);
    dword = dword & 0xFFFFFF;	  
    dword = dword | (buffer[offset+3]<<24);
  }
  else
  {
    dword = buffer[offset+3];
    dword = dword & 0xFF;
    dword = dword | (buffer[offset+2]<<8);
    dword = dword & 0xFFFF;	  	  
    dword = dword | (buffer[offset+1]<<16);
    dword = dword & 0xFFFFFF;	  	  
    dword = dword | (buffer[offset]<<24);
  }
  return dword;
}

unsigned short int two_return_int (unsigned char *buffer,int offset,bool littleendian)
{
 unsigned short int dword;
  if (!littleendian)
  {
    dword = buffer[offset];
    dword = dword & 0xFF;
    dword = dword | (buffer[offset+1]<<8);
  }
  else
  {
    dword = buffer[offset+1];
    dword = dword & 0xFF;
    dword = dword | (buffer[offset]<<8);
  }
  return dword;
}





int strlen (char *string)
{
  int i;
  for (i=0;string[i]!='\0';i++);
  return i;
}

int strcmp (const char *string1,const char *string2)
{
  while ((*string1 == *string2) && (*string1) && (*string2))
  { string1++; string2++; }
  return (*string1)-(*string2);
}

void strcpy (char *string1,char *string2)
{
  while (*string2!='\0')
  { *string1 = *string2; string1++; string2++; }
  *string1 = '\0';
}
void memcpy (char *source,char *destination,int count)
{
  for (int j=0;j<count;j++)
    *(destination+j) = *(source+j);
}

void memset(char *base,char initvalue,unsigned int count)
{
  for (int j=0;j<count;j++)
    *(base+j) = initvalue;
}

bool isalpha(char c)
{
  if(((c>64)&&(c<91))||((c>96)&&(c<123))) return 1;
  else return 0;
}

bool isnum(char c)
{
  if((c>47)&&(c<57)) return 1;
  else return 0;
}

int strtoi(const char *string)
{
  bool neg=0,flag=0;
  int num=0;
  for(int i=0;i<strlen((char *)string);i++)
  {
    if((i==0)&&(string[i]=='-')) neg=1;
    else if(isnum(string[i]))
    {
      num=(num*10)+(string[i]-48);
    }
    else flag=1;
  }
  if(flag) return 0;
  else if(neg) return(-num);
  else return(num);
}

void strcat (char *string1,char *string2)
{
  while (*string1!='\0') string1++;
  while (*string2!='\0')
  { *string1 = *string2; string1++; string2++; }
  *string1 = '\0';
}

void sysinfo ()
{
  disp1.printf ("\nProcessor family : i386");	
  disp1.printf ("\nTotal Usable RAM : %d MB",pm.max_frames>>8);
  disk_info *temp = dih;
  while (temp)
  {
    disp1.printf ("\n%s (%d GB)",temp->name,temp->sectors>>21);
    temp = temp -> next;
  }
  disp1.printf ("\n\n");

}

void matrix ()
{
  disp1.setattrib (0x02);
  disp1.clear_screen ();



 for (int o=0;o<250;o++) 
  for (int i=0;i<24;i++)
    for (int j=0;j<80;j++)
	{



  for (int k=5,l=5;k<10;k++)
  {	
    if (j==k && i==l)goto ju; 
  }

  for (int k=5,l=11;k<10;k++)
  {	
    if (j==k && i==l)goto ju; 
  }

  for (int k=4,l=6;l<11;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=10,l=6;l<11;l++)
  {
    if (j==k && i==l)goto ju;   
  }



  for (int k=18,l=5;k>14;k--)
  {	
    if (j==k && i==l)goto ju; 
  }

  for (int k=18,l=8;k>14;k--)
  {
    if (j==k && i==l)goto ju; 
  }

  for (int k=18,l=11;k>14;k--)
  {
    if (j==k && i==l)goto ju;   
  }


  for (int k=14,l=6;l<8;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=19,l=9;l<11;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=22,l=5;l<12;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=28,l=5;l<12;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=22,l=5;k<=25;l++,k++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=28,l=5;k>=25;l++,k--)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=32,l=5;k<37;k++)
  {	
    if (j==k && i==l)goto ju; 
  }

  for (int k=32,l=11;k<37;k++)
  {	
    if (j==k && i==l)goto ju; 
  }

  for (int k=31,l=6;l<11;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=37,l=6;l<11;l++)
  {
    if (j==k && i==l)goto ju;   
  }


  for (int k=43,l=5;k>39;k--)
  {	
    if (j==k && i==l)goto ju; 
  }

  for (int k=43,l=8;k>39;k--)
  {
    if (j==k && i==l)goto ju; 
  }

  for (int k=43,l=11;k>39;k--)
  {
    if (j==k && i==l)goto ju;   
  }


  for (int k=39,l=6;l<8;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=44,l=9;l<11;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=47,l=5;k<51;l++,k++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=53,l=5;k>49;l++,k--)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=50,l=9;l<12;l++)
  {
    if (j==k && i==l)goto ju;   
  }
  

  for (int k=59,l=5;k>55;k--)
  {	
    if (j==k && i==l)goto ju; 
  }

  for (int k=59,l=8;k>55;k--)
  {
    if (j==k && i==l)goto ju; 
  }

  for (int k=59,l=11;k>55;k--)
  {
    if (j==k && i==l)goto ju;   
  }


  for (int k=55,l=6;l<8;l++)
  {
    if (j==k && i==l)goto ju;   
  }

  for (int k=60,l=9;l<11;l++)
  {
    if (j==k && i==l)goto ju;   
  }

	disp1.gotoxy (j,i);	
	disp1.printf ("%x",((sys_clock+13*i+7*j)&0xf));
 ju:; 
 }
  
}
