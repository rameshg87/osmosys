/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#ifndef VIDEO_DOT_H

#define VIDEO_DOT_H

#include "common.h"

class display
{
  unsigned int cursorx;
  unsigned int cursory;

  unsigned int maxx;
  unsigned int maxy;

  unsigned short int attrib;
  unsigned char *video_base_location;

public:
  display () { }
  void initialize (unsigned int,unsigned int,unsigned int);
  void clear_screen ();
  void putchar (char character);
  void update_cursor ();
  void putstring (const char *string);
  unsigned int getx();    
  unsigned int gety();
  void gotoxy (int xvalue,int yvalue); 
  void gotoend () ;
  void putint_decimal (unsigned int);
  void putint_hexadecimal (unsigned int);
  void setattrib (unsigned char attrib);
  unsigned char getattrib ();
  void printf(const char *, ...);
};

#endif
