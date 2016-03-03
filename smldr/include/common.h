/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.4

********************************************************************************/

//Function for common use declared here...

#ifndef COMMON_DOT_H
#define COMMON_DOT_H

void outportb (unsigned short int,unsigned char);
unsigned char inportb (unsigned short int port);

void outportw (unsigned short int,unsigned short int);
unsigned short int inportw (unsigned short int port);
unsigned int return_dword (char buffer[][512],int i1,int i2);
unsigned short int return_word (char buffer[][512],int i1,int i2);
int strlen (const char *);
int strcmp (const char *,const char*);
void strcpy (char *,char*);
void memcpy (char *,char *,int);


struct oses
{
  int partindex;
  char name[25];
  bool isosmosys;
};

#endif


