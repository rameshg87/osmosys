/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.1

********************************************************************************/

#include <filesys.h>
#include <common.h>
#include <video.h>

/* disk buffer declared in smldr.cpp */
extern char diskbuffer[33][512];

/* fat partition constructor 
   assigns values to various attributes of this partition
   partition number is passed as argument */
fat_partition :: fat_partition (int part_index)
{
  /* get the lba address of this partition by reading from mbr */
  partition_lba = return_partition_lba (part_index);

  /* read vbr for this partition */
  read_sector (partition_lba,0);

  /* various attributes at corresponding offset on vbr */
  sectors_per_cluster = diskbuffer[0][0x0D];
  number_of_reserved_sectors = return_word (diskbuffer,0,0x0E);
  number_of_fat = diskbuffer[0][0x10];
  sectors_per_fat = return_dword (diskbuffer,0,0x24);
  root_dir_cluster = return_dword (diskbuffer,0,0x2C);

  /* calculated attributes */
  cluster_begin_lba = partition_lba + number_of_reserved_sectors + (number_of_fat * sectors_per_fat);
  fat_begin_lba = partition_lba + number_of_reserved_sectors;
  root_dir_lba = cluster_begin_lba + (root_dir_cluster - 0x02) * (sectors_per_cluster);

}

/* this function checks whether the directory entry is present on this cluster
   takes in filename and the disk buffer index on which cluster lies as argument */
int fat_partition :: find_directory_entry (const char *filename,int db_index)
{
  char *pointer;
  int i,j;
  /* loop the whole cluster */
  for (j=0;j<sectors_per_cluster;j++)
  for (i=0;i<16;i++)
  {
    /* get a pointer point to the beginning of the directory entry */

    pointer = & (diskbuffer [db_index+j][i*32]);
    /* check if this is a end of directory entry  */
    if ((*pointer) == 0x00)
     return -1;

    /* put a null character at the end for comparing string */
    *(pointer + 11) = '\0';

    /* compare the filename passed and the filename in the directory entry
       quit if match found */
    if (!strcmp (filename,pointer))
     { j++;
      goto jump_outside_all_loop; }
  }

jump_outside_all_loop:


  /* both the sector number and index within the sector needs to be passed
     because sectors_per_cluster can be greater than 1 */

  return (j-1)*16+i;
}

/* function that returns cluster number of a file when filename, index of cluster
   on the disk buffer and directory cluster number are passed */
int fat_partition :: return_file_cluster (const char *filename,int dir_cluster_db_index,int dir_cluster,int *filesize)
{
  bool done = false;
  int file_cluster,file_index,index_value;
  int db_index = dir_cluster_db_index;

  /* loop until a result is found */
  while (done==false)
  {
    /* check if directory entry of the file is present on this cluster */
    index_value = find_directory_entry (filename,db_index);
    if (index_value < 0)
    { 
      /* end of directory record has been reached and file not yet found hence return -1 */

      done=true; 
      file_cluster=-1;
      break;
    }
    else if (index_value == sectors_per_cluster * 16)  
    {


      /* the cluster was searched completely but file was not found
         we need to get the next cluster in and search for the directory entry again*/
      dir_cluster = find_chain_cluster ((unsigned int)dir_cluster);
      read_cluster (dir_cluster, db_index);
      continue;
    }
    else
    {
      /* we found the entry for the file */
      done=true; 
      /* find out the higher 16 bits of cluster number */
      file_cluster=return_word (diskbuffer,db_index+(index_value*0x20)/0x200,(index_value*0x20)%0x200+0x14);
      file_cluster |= return_word (diskbuffer,db_index+(index_value*0x20)/0x200,(index_value*0x20)%0x200+0x1A);
 
      /* return the filesize if asked */
      if (filesize != 0)
        *filesize=return_dword (diskbuffer,db_index+index_value/128,(index_value%128)*32+0x1C);

      break;
    }
  }
  return file_cluster;
}

/* this function finds out the next cluster of the file given the current
   cluster number by looking at fat 1 */
int fat_partition :: find_chain_cluster (int cluster)
{
  /* find out the sector on which the chain value lies */
  int chain_lba = fat_begin_lba + (cluster/128);
  read_sector (chain_lba,9);

  /* find out the chain value */
  unsigned int return_value = return_dword (diskbuffer, 9, 4*(cluster & 0x7F));

  return return_value;
}

/* this function reads clusters from disk given cluster number 
   and disk buffer index to which cluster is to be loaded */
void fat_partition :: read_cluster (int cluster,int dbindex)
{
  int j,cluster_lba;

  /* get cluster lba address */
  cluster_lba = cluster_begin_lba + (cluster - 0x02) * (sectors_per_cluster);

  /* read all the sectors within the cluster */
  for (j=0;j<sectors_per_cluster;j++)
    read_sector (cluster_lba+j,dbindex+j);
}

/* this function reads a sector given the lba address and disk buffer index
   to which the sector is to be loaded */
void read_sector (int lba_address,int index)
{
  int idx;
  unsigned short int tmpword;

  /* tell controller that we are going to give lba28 address */
  outportb (0x1F1,0x00);

  outportb (0x1F2,0x01);

  /* lba address bits 7-0 */
  outportb (0x1F3,(lba_address)&0xFF);

  /* lba address bits 15-8 */
  outportb (0x1F4,(lba_address>>8)&0xFF);

  /* lba address bits 23-16 */
  outportb (0x1F5,(lba_address>>16)&0xFF);

  /* magic bit + drive number (master / slave) + lba address bits 27-24 */
  outportb (0x1F6,0xE0 | ((lba_address>>24) & 0x0F));

  /* read command */
  outportb (0x1F7, 0x20);

  /* wait for the controller to get ready */
  while (!(inportb(0x1F7) & 0x08));

  /* read the sector word by word and store it in disk buffer */
  for (idx = 0; idx < 256; idx++)
  {
    tmpword = inportw(0x1F0);
    diskbuffer [index][idx*2] = (tmpword & 0xFF);
    diskbuffer [index][idx*2+1] = ((tmpword>>8) & 0xFF);
  }
}

/* this function returns the lba address of a partition by looking at the partition table of the mbr and ebr */
int return_partition_lba (int index)
{
  int lba=0;
  int offset;

    /* read the mbr */
    read_sector (0,0);

    /* calculate offset to the partition */
    offset = 0x1be + (index/8) * 0x10 + 0x8;

    /* get the lba address */
    lba = return_dword (diskbuffer,0,offset);

  /* if the partition is an extended partition */
  if (index%8 != 0)
  {
    int logical_partition_number = index%8,offset;
    printf ("\nllba=%x",lba);
    /* read the first ebr */
    read_sector (lba,0);    
    
    /* chain through the ebrs */ 
    for (int i=1; i< logical_partition_number; i++)
    { 
      offset = return_dword (diskbuffer,0,470);           
      lba = lba + offset;
      read_sector (lba,0);
    }
  
    /* finally we have reached the required ebr */
    lba = lba + return_dword (diskbuffer,0,454);
}

  return lba;
}


