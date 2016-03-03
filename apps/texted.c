#include "syscalls.h"

#define NULL 0

struct node
{
	char *list;
	struct node *next;
	struct node *prev;
};


struct node *start,*end,*disp_start;
int count_of_nodes;
char *filename;

void insert_into_list (struct node *temp)
{
	temp -> next = 0;
	count_of_nodes ++;

	if (start == NULL)
	{
		start = temp;
		temp -> prev = 0;
		end = start;
	}
	else
	{
		end -> next = temp;
		temp -> prev = end;
		end = temp;
	}	
}

void display ()
{
	int i;
	struct node *temp;
	for (i=0,temp=disp_start;temp!=NULL &&  gety ()<24;i++,temp=temp->next)
		print ("%s",temp->list);
	
	if (gety () < 24)
	{
		setattrib (0x71);
		while (gety () < 24)
			print ("\n~");
	}
	setattrib (0x71);	
	print ("FILE : %s",filename);
	setattrib (0x70);
}

void pmain ()
{
	unsigned char *addr,c;
	clrscr ();

retry:
	print ("Enter Filename : ");
	filename = gets ();
	if (file_search (filename,0) != 1)
	{
		print("\nFILE NOT FOUND\n");
		goto retry;
	}
		
	struct node *temp;
	int fid,read_size,i,j,k,lastpos,qwe;
	count_of_nodes=0;
	start = end = 0;
	fid=open(filename,"R");
	setattrib (0x70);

	qwe = 0;

	while(read_size=read(fid,&addr,4095))
	{

		int tmpcount = 0;
		i = j = 0;
		while(read_size)
		{
			lastpos = j;

			for (i=0;addr[j]!='\n' && i<= 80 && read_size!=0 ; i++,j++,read_size--);
			if (addr[j]=='\n'){ j++; read_size --; }
			temp = (struct node*)malloc (sizeof (struct node));
			temp -> list = (char*) malloc (i+2);
			for (k=0;k<=i;k++)
				temp -> list [k] = addr [lastpos+k];
			temp -> list[k+1] = '\0';
			insert_into_list (temp);
		}
		qwe++;
	}

	disp_start = start;

	do 
	{
		clrscr ();
		display ();		
		c = getc ();
		switch (c)
		{
			case 'w' : if (disp_start -> prev) disp_start = disp_start -> prev; break;
			case 's' : 	if (disp_start)
					if (disp_start -> next)
					if (disp_start -> next -> next) 
						{ disp_start = disp_start -> next; break; }
		}
	}while (c != 'q');

	setattrib (0x0f);
	clrscr ();
}

