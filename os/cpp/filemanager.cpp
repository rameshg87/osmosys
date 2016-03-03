/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/

#include <filemanager.h>

extern display disp1;
extern logical_memory klm;
extern logical_memory *current_active_memory;
extern int irq14_flag;
extern int irq15_flag;
extern file_table *ft_head;
extern file_table *ft_ptr;
extern char *cd_name;
extern disk_info *dih,*dit;

void identify_command_result (char dtype)
{
	unsigned short int data_port;
	unsigned short int status_port;
	unsigned char status;

	if (dtype == PRIMARY_MASTER || dtype == PRIMARY_SLAVE)
	{
		data_port = 0x1f0;
		status_port = 0x1f7;
	}		
	else
	{
		data_port = 0x170;
		status_port = 0x177;
	}

	while ( (status & 0x80) && !(status & 0x08))
	  status = inportb (status_port);

	/* allocate a buffer to store details from identify drive command */
	unsigned char *tmpbuffer = (unsigned char*)klm.kdmalloc (512);
	  	  
	  for (int idx = 0; idx < 256; idx++)
	    {	
	      unsigned short int tmpword = inportw(data_port);
	      tmpbuffer [idx*2] = (unsigned char)(tmpword & 0xFF);
	      tmpbuffer [idx*2+1] = (unsigned char)((tmpword>>8) & 0xFF);
	    }
	  
	disk_info *temp = (disk_info*) klm.kdmalloc (sizeof (disk_info));

	switch (dtype)
	{
		case PRIMARY_MASTER:disp1.printf ("\nPrimary Master : ");strcpy (temp -> name,"Primary Master : ");break;
		case PRIMARY_SLAVE:disp1.printf ("\nPrimary Slave : ");strcpy (temp -> name,"Primary Slave : ");break;
		case SECONDARY_MASTER:disp1.printf ("\nSecondary Master : ");strcpy (temp -> name,"Secondary Master : ");break;
		case SECONDARY_SLAVE:disp1.printf ("\nSecondary Slave : ");strcpy (temp -> name,"Secondary Slave : ");break;
	}	  
	
	if (dih == 0)
	{
		
		dih = temp;
		dit = dih;
		dih -> next = 0;
	}
	else
	{
		dit -> next = temp;
		temp -> next = 0;
	}	

	int k;
	for (k=0;temp->name[k] !='\0';k++);
	for (int j=54;j<92;j+=2,k+=2)
		if (tmpbuffer[j] == ' '&&tmpbuffer[j+1] == ' ')break;
		else {disp1.printf ("%c%c",tmpbuffer[j+1],tmpbuffer[j]);
			temp->name[k] = tmpbuffer[j+1];
			temp->name[k+1] = tmpbuffer[j];	}
	temp -> name [k-1] = '\0';


	unsigned int sectors = return_int (tmpbuffer,120,0);
	temp -> sectors = sectors;
	disp1.printf (" [Disk sectors = %d]",sectors);	  
	/* free the buffer */
	klm.kdfree (tmpbuffer,512);
}

void filemanager :: initialize ()
{

  unsigned char status;
  drive *tmp;
  
  /* initialize the head and tail of partition list and device list */
  phead = 0;
  ptail = 0;
  dhead = 0;
  dtail = 0;
  
  /* cached recently used partitions */
  rup1.isvalid = false;
  rup2.isvalid = false;
  
  /* cached recently used files */
  ruf1.isvalid = false;
  ruf2.isvalid = false;
  
  
  number_of_partitions =0;
  number_of_devices = 0;
  
  /* this variable is used for assigning device numbers */
  maxdno = 0;
  
  /* this variable is used for assigning drive letters */
  partition_status=0xffffffff;	//unused drives set to 1
  
  /* DETECT PRIMARY MASTER */
  
  /* test existence of master device on primary ide controller */
  outportb (0x1f6,0xa0);
  
  //simple sleep function
  for (int i=0;i<0xffff;i++);
  
  /* make irq14_flag to wait for irq */
  irq14_flag = -2;
	  
  /* issue IDENTIFY DRIVE command */
  outportb (0x1F7,0xEC);

  /* read the status register */
  inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);inportb (0x1f7);
  status = inportb (0x1f7);
  
  if ((status!=0) && ((status & 0x1) == 0))
    {
	while ( (status & 0x80) && !(status & 0x08))
	  status = inportb (0x1f7);

        tmp = new primarydisk (MASTER,maxdno);
      
	identify_command_result (PRIMARY_MASTER);

	maxdno ++;
	attach_device (tmp,PRIMARY_IDE_DEVICE);
    }
  
  irq14_flag = -1;
/*
  outportb (0x1f6,0xb0);
  //sleep function

  for (int i=0;i<0xffff;i++);

  irq14_flag = -2;

  outportb (0x1F7,0xEC);
  status = inportb (0x1f7);
  
  if (status != 0 && ((status & 0x1) == 0))
    {
	tmp = new primarydisk (SLAVE,maxdno);
	identify_command_result (PRIMARY_SLAVE);
	maxdno ++;
	attach_device (tmp,PRIMARY_IDE_DEVICE);
    }

  irq14_flag = -1;
*/
/*
	
  outportb (0x176,0xa0);
  //sleep function
  for (int i=0;i<0xffff;i++);

	irq15_flag = -2;
  	outportb (0x177,0xA1);

  	inportb (0x177);inportb (0x177);inportb (0x177);inportb (0x177);
  	status = inportb (0x177);

	disp1.printf ("error = %x",status);

	if (status != 0 && ((status & 0x1) == 0))
	{
		tmp = new secondarydisk (MASTER,maxdno);
		identify_command_result (SECONDARY_MASTER);
		maxdno ++;
      		attach_device (tmp,CDROM);
	}

	else 
	{
		irq15_flag = -2;
		outportb (0x177,0xEC);
  
		inportb (0x177);inportb (0x177);inportb (0x177);inportb (0x177);
		status = inportb (0x177);

		if (status != 0 && ((status & 0x1) == 0))
		{

        		tmp = new secondarydisk (MASTER,maxdno);
			identify_command_result (SECONDARY_MASTER);
			maxdno ++;
			attach_device (tmp,SECONDARY_IDE_DEVICE);
	
		}
	}
		
	irq15_flag=-1;
*/
/*  
  outportb (0x176,0xb0);

  //sleep function required
  for (int i=0;i<0xffff;i++);
  irq15_flag = -2;
  outportb (0x177,0xEC);

  inportb (0x177);inportb (0x177);inportb (0x177);inportb (0x177);
  status = inportb (0x177);
  if (status != 0 && ((status & 0x1) == 0))
    {

      tmp = new secondarydisk (SLAVE,maxdno);
	identify_command_result (SECONDARY_SLAVE);
      maxdno ++;
      attach_device (tmp,SECONDARY_IDE_DEVICE);
    }
	irq15_flag=-1;    
  
*/

  device_list *tmp1 = dhead;
  fs *f;
  
  unsigned char *mbr = (unsigned char*)klm.kdmalloc (512);
  unsigned char *ebr = (unsigned char*)klm.kdmalloc (512);;
  unsigned int nlba,flba;

  while  (tmp1 != 0)
    {

      if (tmp1 -> dtype == PRIMARY_IDE_DEVICE || tmp1 -> dtype == SECONDARY_IDE_DEVICE)
	{
	  
	  /* read the mbr of the device */
	  tmp1 -> device -> pioread (0,1,mbr);
	  for (int i=0;i<4;i++)
	    {

	      /* check for a fat32 partition...(0b is FAT32 and 0c is FAT32(LBA) */
	      if (mbr [0x1be + 16*i + 4] == 0xc || mbr [0x1be + 16*i + 4] == 0xb )
		{
		  /* find out the partition begin lba */
		  unsigned int lba = return_int (mbr, 0x1be + 16*i + 8,0);
		  f = new fat32 (lba,tmp1 -> device);
		  attach_partition (f,FAT32);
		}
	      
	      /* check if this is an extended partition */
	      else if (mbr [0x1be + 16*i + 4] == 0xf)
		{
		  unsigned int lba = return_int (mbr, 0x1be + 16*i + 8,0);
		  tmp1 -> device -> pioread (lba,1,ebr);
		  nlba = return_int (ebr,0x1be + 16 * 1 + 8,0);
		  while (nlba != 0)
		    {
		      if (ebr [ 0x1be + 4] == 0xb || ebr [0x1be + 4] == 0xc)
			{
			  flba = return_int (ebr,0x1be + 8,0);
			  f = new fat32 (flba+lba,tmp1 -> device);
			  attach_partition (f,FAT32);
			}
		      lba = lba + nlba;
		      tmp1 -> device -> pioread (lba,1,ebr);
		      nlba = return_int (ebr,0x1be + 16 * 1 + 8,0);
		    }
		  if (ebr [ 0x1be + 4] == 0xb || ebr [0x1be + 4] == 0xc)
		    {
		      flba = return_int (ebr,0x1be + 8,0);
		      f = new fat32 (flba+lba,tmp1 -> device);
		      attach_partition (f,FAT32);
		    }
		}
	    }
	  
	}
/*	else if (tmp1 -> dtype == CDROM)
	{
		f = new iso9660 (tmp1 -> device);
		attach_partition (f,ISO9660);
	}
*/
      tmp1 = tmp1 -> next;
    }
  
  disp1.printf ("\nTotal number of detected devices = %d",maxdno);
  disp1.printf ("\nTotal number of detected partitions = %d",number_of_partitions);
}

int filemanager :: attach_device (drive *d,int type)
{
  number_of_devices ++;
  if (dhead == 0)
    {
      dhead = new device_list;
      dtail = dhead;
      dhead -> dtype = type;
      dhead -> device = d;
      dhead -> next = 0;
      dhead -> prev = 0;
    }
  else
    {
      dtail -> next = new device_list;
      dtail -> next -> prev = dtail;
      dtail = dtail -> next;
      dtail -> dtype = type;		
      dtail -> device = d;
      dtail -> next = 0;
    }
  return 0;
}

int filemanager :: detach_device (int dno)
{
  device_list *tmp = dhead;
  while (tmp != 0)
    {
      if (tmp -> device -> driveno == dno)
	break;
      tmp = tmp -> next;
    }
  
  if (tmp != 0)
    {
      tmp -> next -> prev = tmp -> prev;
      tmp -> prev -> next = tmp -> next;
      switch (tmp -> dtype)
	{
	case PRIMARY_IDE_DEVICE:
	  klm.kdfree (tmp-> device,sizeof (primarydisk));
	case SECONDARY_IDE_DEVICE:
	  klm.kdfree (tmp-> device,sizeof (secondarydisk));
	}
      klm.kdfree (tmp,sizeof (device_list));
      return 0;
    }
  else
    return E_NOTFOUND;
}

int filemanager :: attach_partition (fs* part,int type)
{
  number_of_partitions ++;
  int checker,i;
  char dletter;
  
  for (i=0;i<31;i++)
    {
      checker=1;
      for (int j=0;j<i;j++)
	checker = checker << 1;
      if ((checker & partition_status) != 0)
	{
	  dletter = 97+i;
	  partition_status = partition_status & (~(checker));
	  break;
	}
    }
  
  if (i>26)return E_NO_DRIVE_LETTER;
  
  if (phead == 0)
    {
      phead = new partition_list;
      ptail = phead;
      phead -> partition= part;
      phead -> drive_letter = dletter;
      phead -> ptype = type;
      phead -> next = 0;
      phead -> prev = 0;
    }
  else
    {
      ptail -> next = new partition_list;
      ptail -> next -> prev = ptail;
      ptail = ptail -> next;
      ptail -> ptype = type;
      ptail -> drive_letter = dletter;
      ptail -> partition = part;
      ptail -> next = 0;
    }
  return 0;
}

int filemanager :: detach_partition (char dletter)
{
  partition_list *tmp = phead;
  while (tmp != 0)
    {
      if (dletter == tmp -> drive_letter)
	{
	  tmp -> next -> prev = tmp -> prev;
	  tmp -> prev -> next = tmp -> next;
	  
	  if (rup1.ppointer == tmp)
	    rup1.isvalid = false;
	  else if (rup2.ppointer == tmp)
	    rup2.isvalid = false;
	  
	  klm.kdfree (tmp -> partition,sizeof (fs));
	  klm.kdfree (tmp,sizeof (partition_list));
	  return 0;
	}
    }
  return E_NOTFOUND;
}


bool filemanager :: create_file (char* name)
{
  char drivename = name[0];
  fs *partition;
  partition_list *temp;
  
  
  if (((rup1.isvalid == true) && (rup1.ppointer -> drive_letter == drivename)))
    {
      rup1.count ++;
      partition = rup1.ppointer -> partition;
    }
  else if (((rup2.isvalid == true) && (rup2.ppointer -> drive_letter == drivename)))
    {
      rup2.count ++;
      partition = rup2.ppointer -> partition;		
    }
  
  else 
    {
      temp = phead;
      while (temp != 0)
	{
	  if (temp -> drive_letter == drivename)
	    {
	      partition = temp -> partition;
	      /* change recently used partition */
	      if ((rup1.count < rup2.count) || (rup1.isvalid == false))
		{
		  rup1.ppointer=temp;
		  rup1.isvalid=true;
		  rup1.count=1;
		}
	      else
		{
		  rup2.ppointer=temp;
		  rup2.isvalid=true;
		  rup2.count=1;
		}
	      break;
	    }
	  temp = temp -> next;
	}
      if (temp == 0)
	return 0;
    }
  name += 2;
  return partition -> fcreate (name);
}	


int filemanager :: open (char* name,char* mode)
{
  char drivename = name[0];
  fs *partition;
  partition_list *temp;
  
  if (((rup1.isvalid == true) && (rup1.ppointer -> drive_letter == drivename)))
    {
      rup1.count ++;
      partition = rup1.ppointer -> partition;
    }
  else if (((rup2.isvalid == true) && (rup2.ppointer -> drive_letter == drivename)))
    {
      rup2.count ++;
      partition = rup2.ppointer -> partition;		
    }
  
  else 
    {
      temp = phead;
      while (temp != 0)
	{
	  if (temp -> drive_letter == drivename)
	    {
	      partition = temp -> partition;
	      /* change recently used partition */
	      if ((rup1.count < rup2.count) || (rup1.isvalid == false))
		{
		  rup1.ppointer=temp;
		  rup1.isvalid=true;
		  rup1.count=1;
		}
	      else
		{
		  rup2.ppointer=temp;
		  rup2.isvalid=true;
		  rup2.count=1;
		}
	      break;
	    }
	  temp = temp -> next;
	}
      if (temp == 0)
	return -1;
    }
  name += 2;
  return partition -> fopen (name,mode);
}	

unsigned int filemanager :: read (unsigned int fid,unsigned char** buffer,unsigned int count)
{
  unsigned char *tmp;
  file_table *fptr = ft_head -> next;
 


  /* search recently used files */
  if (((ruf1.isvalid == true) && (ruf1.fid == fid)))
    {
      ruf1.count ++;
      fptr = ruf1.fpointer;
    }
  else if (((ruf2.isvalid == true) && (ruf2.fid == fid)))
    {
      ruf2.count ++;
      fptr = ruf2.fpointer;		
    }
  else
    {
      /* search for file in linked list */
      while (fptr)
	{
	  if (fptr -> fid == fid)
	    {
	      /* enter the file into recently cached item */
	      if ((ruf1.count < ruf2.count) || (ruf1.isvalid == false))
		{
		  ruf1.fpointer=fptr;
		  ruf1.isvalid=true;
		  ruf1.count=1;
		}
	      else
		{
		  ruf2.fpointer=fptr;
		  ruf2.isvalid=true;
		  ruf2.count=1;
		}
	      break;
	    }
	  fptr = fptr -> next;
	}
    }


  
  if (fptr == 0)
    return 0;


  
  if (current_active_memory == &klm)
	{
		return fptr -> partition -> fread (fptr,buffer,count);

	}
  else
    {
      unsigned char **buf;

      unsigned int cnt = fptr -> partition -> fread (fptr,buf,count);

      if (cnt == 0) return 0;
      unsigned int ret_value = cnt;
      unsigned char *dest_buffer = (unsigned char*)(current_active_memory -> kdmalloc (cnt));

      unsigned char *src_buffer = *buf;
      *buffer = dest_buffer;
      while (cnt>=4096)
	{
	  current_active_memory -> map ((unsigned int)dest_buffer,
					klm.get_physical_address ((unsigned int)src_buffer),0x3);
	  klm.set_page_attribute ((unsigned int)src_buffer,0x0);
	  dest_buffer += 4096;
	  src_buffer += 4096;
	  cnt=cnt-4096;
	}

      tmp = src_buffer;
      for (int i=0;i<cnt;i++)
	{
	  (*dest_buffer) = (*src_buffer);
	  dest_buffer ++;
	  src_buffer ++;
	}

//      klm.kdfree (tmp,cnt);

	return ret_value;	
    }
}

unsigned int filemanager :: write (unsigned int fid,unsigned char* buffer,unsigned int count)
{
  unsigned int cnt;

  file_table *fptr = ft_head -> next;
  
  /* search recently used files */
  if (((ruf1.isvalid == true) && (ruf1.fid == fid)))
    {
      ruf1.count ++;
      fptr = ruf1.fpointer;
    }
  else if (((ruf2.isvalid == true) && (ruf2.fid == fid)))
    {
      ruf2.count ++;
      fptr = ruf2.fpointer;		
    }
  else
    {
      /* search for file in linked list */
      while (fptr)
	{
	  if (fptr -> fid == fid)
	    {
	      /* enter the file into recently cached item */
	      if ((ruf1.count < ruf2.count) || (ruf1.isvalid == false))
		{
		  ruf1.fpointer=fptr;
		  ruf1.isvalid=true;
		  ruf1.count=1;
		}
	      else
		{
		  ruf2.fpointer=fptr;
		  ruf2.isvalid=true;
		  ruf2.count=1;
		}
	      break;
	    }
	  fptr = fptr -> next;
	}
    }
  
  if (fptr == 0)
    return 0;
  
  return fptr -> partition -> fwrite (fptr,buffer,count);
}


bool filemanager :: close (unsigned int fid)
{
  file_table *fptr = ft_head -> next;
  
  /* search recently used files */
  if (((ruf1.isvalid == true) && (ruf1.fid == fid)))
    {
      ruf1.count ++;
      fptr = ruf1.fpointer;
    }
  else if (((ruf2.isvalid == true) && (ruf2.fid == fid)))
    {
      ruf2.count ++;
      fptr = ruf2.fpointer;		
    }
  else
    {
      /* search for file in linked list */
      while (fptr)
	{
	  if (fptr -> fid == fid)
	    break;
	  fptr = fptr -> next;
	}
    }
  
  if (fptr == 0)
    return 0;
  
  return fptr -> partition -> fclose (fptr);
}

int filemanager :: seek (unsigned int fid,int offset,int from)
{
  file_table *fptr = ft_head -> next;
  
  /* search recently used files */
  if (((ruf1.isvalid == true) && (ruf1.fid == fid)))
    {
      ruf1.count ++;
      fptr = ruf1.fpointer;
    }
  else if (((ruf2.isvalid == true) && (ruf2.fid == fid)))
    {
      ruf2.count ++;
      fptr = ruf2.fpointer;		
    }
  else
    {
      
      /* search for file in linked list */
      while (fptr)
	{
	  if (fptr -> fid == fid)
	    {
	      /* enter the file into recently cached item */
	      if ((ruf1.count < ruf2.count) || (ruf1.isvalid == false))
		{
		  ruf1.fpointer=fptr;
		  ruf1.isvalid=true;
		  ruf1.count=1;
		}
	      else
		{
		  ruf2.fpointer=fptr;
		  ruf2.isvalid=true;
		  ruf2.count=1;
		}
	      break;
	    }
	  fptr = fptr -> next;
	}
      if (fptr == 0)
	return -1;
    }
  return fptr->partition->fseek (fptr,offset,from);
}


bool filemanager :: delete_file (char* name)
{
  char drivename = name[0];
  fs *partition;
  partition_list *temp;
  
  
  if (((rup1.isvalid == true) && (rup1.ppointer -> drive_letter == drivename)))
    {
      rup1.count ++;
      partition = rup1.ppointer -> partition;
    }
  else if (((rup2.isvalid == true) && (rup2.ppointer -> drive_letter == drivename)))
    {
      rup2.count ++;
      partition = rup2.ppointer -> partition;		
    }
  
  else 
    {
      temp = phead;
      while (temp != 0)
	{
	  if (temp -> drive_letter == drivename)
	    {
	      partition = temp -> partition;
	      /* change recently used partition */
	      if ((rup1.count < rup2.count) || (rup1.isvalid == false))
		{
		  rup1.ppointer=temp;
		  rup1.isvalid=true;
		  rup1.count=1;
		}
	      else
		{
		  rup2.ppointer=temp;
		  rup2.isvalid=true;
		  rup2.count=1;
		}
	      break;
	    }
	  temp = temp -> next;
	}
      if (temp == 0)
	return 0;
    }
  name += 2;
  return partition -> fdelete (name);
}


bool filemanager :: list (char* name)
{
  char drivename = name[0];
  fs *partition;
  partition_list *temp;
  
  
  if (((rup1.isvalid == true) && (rup1.ppointer -> drive_letter == drivename)))
    {
      rup1.count ++;
      partition = rup1.ppointer -> partition;
    }
  else if (((rup2.isvalid == true) && (rup2.ppointer -> drive_letter == drivename)))
    {
      rup2.count ++;
      partition = rup2.ppointer -> partition;		
    }
  
  else 
    {
      temp = phead;
      while (temp != 0)
	{
	  if (temp -> drive_letter == drivename)
	    {
	      partition = temp -> partition;
	      /* change recently used partition */
	      if ((rup1.count < rup2.count) || (rup1.isvalid == false))
		{
		  rup1.ppointer=temp;
		  rup1.isvalid=true;
		  rup1.count=1;
		}
	      else
		{
		  rup2.ppointer=temp;
		  rup2.isvalid=true;
		  rup2.count=1;
		}
	      break;
	    }
	  temp = temp -> next;
	}
      if (temp == 0)
	return 0;
    }
  name += 2;
  return partition -> flist (name);
}


bool filemanager :: create_dir (char* name)
{
  char drivename = name[0];
  fs *partition;
  partition_list *temp;
  
  
  if (((rup1.isvalid == true) && (rup1.ppointer -> drive_letter == drivename)))
    {
      rup1.count ++;
      partition = rup1.ppointer -> partition;
    }
  else if (((rup2.isvalid == true) && (rup2.ppointer -> drive_letter == drivename)))
    {
      rup2.count ++;
      partition = rup2.ppointer -> partition;		
    }
  
  else 
    {
      temp = phead;
      while (temp != 0)
	{
	  if (temp -> drive_letter == drivename)
	    {
	      partition = temp -> partition;
	      /* change recently used partition */
	      if ((rup1.count < rup2.count) || (rup1.isvalid == false))
		{
		  rup1.ppointer=temp;
		  rup1.isvalid=true;
		  rup1.count=1;
		}
	      else
		{
		  rup2.ppointer=temp;
		  rup2.isvalid=true;
		  rup2.count=1;
		}
	      break;
	    }
	  temp = temp -> next;
	}
      if (temp == 0)
	return 0;
    }
  name += 2;
  return partition -> fcreate_dir (name);
}


bool filemanager :: cd (char* name)
{
  char drivename = name[0];
  fs *partition;
  partition_list *temp;
  
  
  if (((rup1.isvalid == true) && (rup1.ppointer -> drive_letter == drivename)))
    {
      rup1.count ++;
      partition = rup1.ppointer -> partition;
    }
  else if (((rup2.isvalid == true) && (rup2.ppointer -> drive_letter == drivename)))
    {
      rup2.count ++;
      partition = rup2.ppointer -> partition;		
    }
  
  else 
    {
      temp = phead;
      while (temp != 0)
	{
	  if (temp -> drive_letter == drivename)
	    {
	      partition = temp -> partition;
	      /* change recently used partition */
	      if ((rup1.count < rup2.count) || (rup1.isvalid == false))
		{
		  rup1.ppointer=temp;
		  rup1.isvalid=true;
		  rup1.count=1;
		}
	      else
		{
		  rup2.ppointer=temp;
		  rup2.isvalid=true;
		  rup2.count=1;
		}
	      break;
	    }
	  temp = temp -> next;
	}
      if (temp == 0)
	return 0;
    }
  name += 2;
  return partition -> fcd (name);
}

bool filemanager :: file_search (char* name,unsigned char file)
{
  char drivename = name[0];
  fs *partition;
  partition_list *temp;
  
  if (((rup1.isvalid == true) && (rup1.ppointer -> drive_letter == drivename)))
    {
      rup1.count ++;
      partition = rup1.ppointer -> partition;
    }
  else if (((rup2.isvalid == true) && (rup2.ppointer -> drive_letter == drivename)))
    {
      rup2.count ++;
      partition = rup2.ppointer -> partition;		
    }
  
  else 
    {
      temp = phead;
      while (temp != 0)
	{
	  if (temp -> drive_letter == drivename)
	    {
	      partition = temp -> partition;
	      /* change recently used partition */
	      if ((rup1.count < rup2.count) || (rup1.isvalid == false))
		{
		  rup1.ppointer=temp;
		  rup1.isvalid=true;
		  rup1.count=1;
		}
	      else
		{
		  rup2.ppointer=temp;
		  rup2.isvalid=true;
		  rup2.count=1;
		}
	      break;
	    }
	  temp = temp -> next;
	}
      if (temp == 0)
	return 0 ;
    }
  name += 2;
  return partition -> fs_search (name,file);
}	

