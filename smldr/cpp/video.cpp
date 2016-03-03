/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.1

********************************************************************************/

#include  <video.h>

  //bring cursor to the start
 int  cursorx = 0;
 int  cursory = 0;

  //define the maximum values
 int  maxx=80;
 int  maxy=25;

 unsigned short int attrib=0x0F;

  //base location of the text-mode framebuffer
 unsigned char *video_base_location = (unsigned char*)0xb8000;


//updates the visible cursor
void  update_cursor ()
{
  int offset = cursory * 80 + cursorx;

  //To select 14th index register for operation (that is where y value is stored)
  outportb (0x3D4, 14); 			

  outportb (0x3D5, offset/256);	
  
  //To select 15th index register for operation (that is where x value is stored)
  outportb (0x3D4, 15); 			
  
  //write offset value of the cursor
  outportb (0x3D5, offset % 256);  	
}


//brings the cursor to the specified position
void  gotoxy (int xvalue,int yvalue)
{
  cursorx = xvalue;
  cursory = yvalue;
  update_cursor ();
}

//takes the cursor nearer to the end of the line
void  gotoend ()
{
  cursorx = 75;
  update_cursor ();
}

//clears the screen - to be franc fills the screen with white spaces
void  clear_screen ()
{
  int i;
  cursorx = 0;
  cursory = 0;

  for (i=0;i< (maxx * maxy);i++)
  {
    *(video_base_location + 2*i) = ' ';
    *(video_base_location + 2*i +1) = attrib;
  }

}

//puts the specified character on the screen and updates the cursor of the display
void  putchar (char character)
{
  if (character == '\n')
  {
    cursory++;
    cursorx = 0;
  }
  else
  {
    *(video_base_location + ( maxx * cursory + cursorx) * 2) = character;
    *(video_base_location + ( maxx * cursory + cursorx) * 2 + 1) = attrib;
    cursorx ++;
  }

  //if we have reached end of line
  if (cursorx == maxx)
  {
    cursorx = 0;
    cursory ++;
  }

  //if we have reached the bottom of the screen
  if (cursory == maxy)
  {
    clear_screen ();
    cursory = 0;
  }

  update_cursor ();
}

//put a string on the display
void  putstring (const char *string)
{
  //loop until null character is found
  for ( ; *(string) != '\0';string++)
    putchar (*string);
  update_cursor ();
}

//function to print decimal value of an unsigned integer
void putint_decimal (unsigned int number)
{
  //to store remainders
  int rem;

  //to store the digits 1-by-1
  int arr[10];

  //to store the count of digits
  int count=0;

  //loop until number is zero
  while (number)
  {

    rem = number % 10;

    //store the ascii value of the digit in the array
    arr[count++] = rem + 48;

    number = number / 10;
  }

  //put the digits on the screen
  for (rem = count -1;rem>=0;rem--)
    putchar(arr[rem]);
}

void  putint_hexadecimal (unsigned int number)
{
  int rem;
  int arr[8];
  int count=0;

  if (!number)
  {
    putchar ('0');
    return;
  }

  while (number)
  {
    rem = number % 16;

    //store the ascii value of the digit in the array
    if (rem > 9)
      arr[count++] = rem + 55;
    else
      arr[count++] = rem + 48;

    number = number / 16;
  }

  //put the digits on the screen
  for (rem = count -1;rem>=0;rem--)
    putchar(arr[rem]);
}

void printf(const char *cstring, ...)
{
 int i;
 __builtin_va_list vl;
 __builtin_va_start(vl,cstring);
 for(i=0;cstring[i]!='\0';i++)
  {
   if(cstring[i]=='%'&&cstring[i+1]=='c')
     {
      int temp=__builtin_va_arg(vl,int);
      putchar((char)temp);  
      i++;
     }
     else if(cstring[i]=='%'&&cstring[i+1]=='s')
     {
      char *temp=__builtin_va_arg(vl,char *);
      putstring(temp);  
      i++;
     }
     else if(cstring[i]=='%'&&cstring[i+1]=='d')
     {
      int temp=__builtin_va_arg(vl,int);
      putint_decimal(temp);  
      i++;
     }
     else if(cstring[i]=='%'&&cstring[i+1]=='x')
     {
      int temp=__builtin_va_arg(vl,int);
      putint_hexadecimal(temp);  
      i++;
     }
   else
    putchar(cstring[i]);
  }
 __builtin_va_end(vl);
}

