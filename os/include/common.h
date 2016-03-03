/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/

//Function for common use declared here...

#ifndef COMMON_DOT_H
#define COMMON_DOT_H

void outportb (unsigned short int,unsigned char);
unsigned char inportb (unsigned short int port);

void outportw (unsigned short int,unsigned short int);
unsigned short int inportw (unsigned short int port);
unsigned int return_int (unsigned char *buffer,int offset,bool littleendian);
int strlen (char *);
int strcmp (const char *,const char*);
void strcpy (char *,char*);
void memcpy (char *,char *,int);
void memset(char *,char,unsigned int);
bool isalpha(char);
bool isnum(char);
int strtoi(const char *);
unsigned short int two_return_int (unsigned char *buffer,int offset,bool littleendian);
void strcat (char *,char *);
void sysinfo ();
void matrix ();

#endif


