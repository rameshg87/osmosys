#ifndef SYSCALLS

#define SYSCALLS

extern void print (const char *,...);
extern void putc (char);
extern void gotoxy (int,int);
extern void clrscr ();
extern int gety ();
extern void setattrib (char);

extern void create_process ();

extern char getc ();
extern int geti ();
extern char *gets ();

extern void* malloc (int);
extern void free (void*,int);

extern int open (char*,char*);
extern int read (int,unsigned char**,int);
extern int write(int,unsigned char*,int);
extern int close (int);
extern int seek (int,int,int);
extern int mkfile (char *);
extern int mkdir (char *);
extern int rm (char *);
extern char *cd ();
extern void ls();
extern char file_search(char *,unsigned char);

extern int strlen (char *);
extern void strcpy (char *,char *);
extern void strcat (char *,char *);
extern unsigned char *strcmp (char *,char *);

extern void sysinfo ();

#endif
