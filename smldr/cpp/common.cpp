/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.4

********************************************************************************/

#include <common.h>
#include <video.h>

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

unsigned int return_dword (char buffer[][512],int i1,int i2)
{
  unsigned int dword = (unsigned int)buffer[i1][i2];
  dword = dword & 0xFF;
  dword = dword | (buffer[i1][i2+1]<<8);
  dword = dword & 0xFFFF;
  dword = dword | (buffer[i1][i2+2]<<16);
  dword = dword & 0xFFFFFF;
  dword = dword | (buffer[i1][i2+3]<<24);
  return dword;
}

unsigned short int return_word (char buffer[][512],int i1,int i2)
{
  unsigned short int word = buffer[i1][i2];
  word = word & 0xFF;
  word = word | (buffer[i1][i2+1]<<8);
  return word;
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
  while (*string2)
  { *string1 = *string2; string1++; string2++; }
  *string1 = '\0';
}

void memcpy (char *source,char *destination,int count)
{
  for (int j=0;j<count;j++)
    *(destination+j) = *(source+j);
}
