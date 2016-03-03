/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.1

********************************************************************************/


#ifndef VIDEO_DOT_H

#define VIDEO_DOT_H

#include "common.h"

void clear_screen ();
void putchar (char character);
void update_cursor ();
void putstring (const char *string);    
void gotoxy (int xvalue,int yvalue);
void gotoend () ;
void putint_decimal (unsigned int);
void putint_hexadecimal (unsigned int);
void printf(const char *cstring, ...);

#endif
