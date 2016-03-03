/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.3

********************************************************************************/

#include <video.h>		// to call video functions
#include <idt.h>		// idt manipulation functions
#include <filesys.h>
#include <common.h>

/* storage location for idt entries each 8 BYTES */
extern struct idt_entry idt_entries[256];

extern "C" unsigned int common_isr;
extern "C" unsigned int idtstructure;

/* function that enters real mode and chainloads another os */
extern "C" void jmp_to_7C00 ();

/* function that jumps to 0x100000 when osmosys kernel is loaded */
extern "C" void start_osmosys ();

/* function that copies kernel cluster by cluster */
extern "C" void memcpy_kernel (int,int);

/* variable that indicates the currently highlighted option */
int current_option;

/* to set the attribute of the characters displayed on screen */
extern int attrib;

/* variable that says whether timer is active */
extern bool is_timer_active;

/* count for timer */
extern int timer;

/* structure that holds details about os(s) */
oses os[10];
int oscount;
int timeout;

char diskbuffer[33][512];

void print_options ()
{
  int i;
  clear_screen ();
  putstring ("SMLDR Smart Loader ...\n");
  gotoxy (0,3);
  for (i=0;i<oscount;i++)
  {
    if (i==current_option)attrib=0x70;
    printf ("    %s    \n",os[i].name);  
    attrib=0x0f;
  }
  if (is_timer_active == true)
    printf ("\nTimer : %d",timer/18);
}

void bootosmosys (int part_index)
{
  int cluster,filesize,to_read;
  int total_sectors,total_clusters;
  unsigned int count;

  /* create a new object of fat partition of osmosys */
  fat_partition part1(part_index);

  /* read the root directory cluster of this partition */
  part1.read_cluster (part1.root_dir_cluster,1);


  /* find the cluster of the directory osmosys */
  cluster = part1.return_file_cluster ("OSMOSYS    ",1,part1.root_dir_cluster,0);

  /* if directory entry not found */


  if (cluster == 0xFFFFFFFF)
  {
    putstring ("UNABLE TO BOOT OSMOSYS. DIRECTORY ENTRY MISSING");
    return;
  }

  /* read the directory entry into buffer 1*/
  part1.read_cluster (cluster,1);
  
  /* find the cluster entry of kernel */
  cluster = part1.return_file_cluster ("KERNEL     ",1,cluster,&filesize);  

  /* if file was not found */
  if (cluster == 0xFFFFFFFF)
  {
    putstring ("UNABLE TO BOOT OSMOSYS. KERNEL MISSING");
    return;
  }

  /* read the first cluster of file into buffer */
  part1.read_cluster (cluster,1);
  printf ("\nfirst cluster = %x",cluster);

  /* total sectors which the file consumes */
  total_sectors=filesize/512;

  /* this keeps of count of how many sectors of file has been loaded in the loop */
  count=total_sectors;
     printf ("\nread count = %d",count);

  /* total clusters which the file consumes */
  total_clusters=filesize/(512*part1.sectors_per_cluster);
 
  printf ("\ntotal clusters = %d",total_clusters);
  int j;
  /* loop that loads the file completely sector by sector */
  for(int i=0;i<=total_clusters;i++)
    {
   
    /* if there is a complete cluster to be loaded */
    if(count>=part1.sectors_per_cluster)
      to_read=part1.sectors_per_cluster;
    else
      to_read=count;
  
    printf ("\n%x %x %x %x\n",diskbuffer[1][0],diskbuffer[1][1],diskbuffer[1][2],diskbuffer[1][3]);
 
     /* load the file at memory addresses higher then 0x100000 (1MB) */
     for(j=0;j<=to_read;j++)
        memcpy_kernel ((unsigned int)(&diskbuffer[1+j][0]),(i*part1.sectors_per_cluster+j)*512);

     /* find cluster chain */
     cluster=part1.find_chain_cluster(cluster);
     printf ("\nchain cluster = %x",cluster);

     /* if cluster chain over then exit */
     if(cluster==0xffffffff)
       break;

     part1.read_cluster(cluster,1);
     count=count-to_read;
     printf ("\nread count = %d",count);
   }

   printf ("\nstarting osmosys, filesize = %d\n",filesize);

   unsigned char *t1 = (unsigned char*) (0x100000+8191);
   unsigned char *t2 = (unsigned char*) (0x100000+8192);
   unsigned char *t3 = (unsigned char*) (0x100000+8193);
   printf ("53 = %x\n357 = %x\n613=%x\n",*t1,*t2,*t3);


  /* make a long jump to start of the kernel */
   start_osmosys();

}	

void chainload_os (int part_index)
{
  int partition_lba = return_partition_lba (part_index);


  read_sector (partition_lba,0);
/*  for (int j=0;j<512;j++)
  { putint_hexadecimal (0xFF & diskbuffer[0][j]); putchar (' '); }
*/
  memcpy ((&diskbuffer[0][0]),(char*)0x7C00,512);
  
  jmp_to_7C00 ();
}

int return_integer (char temp[])
{
  int j,value=0;
  
  for (j=0;temp[j]!='\0';j++)
    value = value *10 + (int (temp [j]) - 48);

  return value;

}

void kmain ()
{

  is_timer_active = false;

  idt_install ();
  clear_screen ();
  int cluster;
	
  
  /* create a new object of fat partition of osmosys */
  fat_partition part1(0);

  /* read the root directory cluster of this partition */
  part1.read_cluster (part1.root_dir_cluster,1);

  int filesize;

  /* find the cluster of the directory osmosys */
  cluster = part1.return_file_cluster ("SMCONF     ",1,part1.root_dir_cluster,&filesize);

  /* if file is not found */
  if (cluster == 0xFFFFFFFF)
  {
    putstring ("CONFIGURATION FILE MISSING");
    for (;;);
  }

  char temp[30];
  int tmpcount=0,code;
  int count=0;
  oscount=0;
  part1.read_cluster (cluster,1);
 
  while (count < filesize)
  {
    if (diskbuffer[1][count] == '=')
    {
      temp[tmpcount]='\0';
      if (strcmp (temp,"timeout") == 0)
	code=0;
      else if (strcmp (temp,"default") == 0)
	code=1;
      else if (strcmp (temp,"osmosys") == 0)
	code=2;
      else if (strcmp (temp,"chloados") == 0)
	code=3;
	count++;
      tmpcount=0;
    }
    else if (diskbuffer[1][count] == '\n')
    {
      temp[tmpcount]='\0';      
	count++;
      if(code == 0)
	{
		timeout = return_integer (temp);
		timer = timeout * 18;
	}
      else if(code == 1)
	{
		current_option = return_integer (temp);
	}
      else if(code == 2)
	{
		strcpy (os[oscount].name,temp);
		oscount++;
	}
      else if(code == 3)
	{
		strcpy (os[oscount].name,temp);
		oscount++;
	}
      tmpcount=0;
    }
    else if (diskbuffer[1][count] == ',')
    {
      temp[tmpcount]='\0';

      if (code == 3)
      {
	os[oscount].isosmosys = false;
	os[oscount].partindex = return_integer (temp);
      }
      else if (code == 2)
      {
	os[oscount].isosmosys = true;
	os[oscount].partindex = return_integer (temp);
      }

      count++;
      tmpcount=0;
    }
    else
    {
      temp[tmpcount++]=diskbuffer[1][count++];
    }

  }
  is_timer_active = 1;

  print_options ();
  __asm__("sti");
  for (;;);
}
