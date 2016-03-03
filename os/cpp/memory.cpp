/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/



#include <memory.h>

extern display disp1;
extern physical_memory pm;
extern logical_memory klm;
extern logical_memory *current_active_memory;
extern unsigned char count;

extern "C" void load_ptbr (unsigned int);
extern int mflag;

/**********************************************************************************

		PHYSICAL MEMORY CLASS FUNCTIONS

***********************************************************************************/

/* This function returns the power of a number */
unsigned int power(unsigned short int number,unsigned short int power)
{
  unsigned int result=1;

  for(int i=0;i<power;i++)
    result=result*number;

  return result;
}

/* constructor for physical memory */
void physical_memory:: initialize ()
{
 
  /* begin the searching after reserved frames */
  last_found_character=RESERVED_FRAMES/8;

	rlast_found_character=RESERVED_ALLOCATION_START/8;
	
  /* reserve the frames used by the kernel */
  memset((char*)bitmap,0xff,RESERVED_ALLOCATION_START/8);


  unsigned short int *temp = (unsigned short int*)0xA000;
  max_frames = *temp;  

  /* bios interrupt e801 cannot get system memory */
  if (max_frames == 0xFFFF)
  {
    disp1.printf ("\nCannot detect system memory size. Defaulting it to 32MB\n",max_frames);
    max_frames = 32;
  }  
  else
  disp1.printf ("\nTotal Physical Memory : %d MB",max_frames);
  
  /* the amount of ram is returned in mb, but we have to get the amount of 4KB pages */
  max_frames=max_frames<<8;
  
  /* mark all other frames as unused */
  memset((char*)bitmap+RESERVED_ALLOCATION_START/8,0,max_frames - RESERVED_ALLOCATION_START/8);
  
  /* stack top as stack size */
  stack_top = STACK_SIZE;

  /* fill in the stack */
  for(int i=0;i<STACK_SIZE;i++)
    {
      /* find the offset of character where search found a free frame , the actual character is updated
 	 in last_found_frame */
      unsigned int offset_into_character=search();

      /* put the absolute frame number into the stack */
      stack[--stack_top]=last_found_character*8+offset_into_character;

      /* update the bitmap */  
      bitmap [last_found_character] |= power (2,offset_into_character);
    }
  
}

unsigned int physical_memory :: rallocate ()
{
	int j;
	for (int i=rlast_found_character;i<RESERVED_FRAMES/8;i++)
	{
		if (bitmap[i] != 0xff)
		{
			for(j=0;j<8;j++)
			{
				unsigned char backup=bitmap[i];
				backup=backup>>j;
				backup&=0x1;
				if(!backup) break;
			}
			last_found_character=i; 
			bitmap [i] = bitmap [i] | power (2,j);
			return (i*8+j)*PAGE_SIZE; 
		}
	}
	
	for (int i=RESERVED_ALLOCATION_START/8;i<rlast_found_character;i++)
	{
		if (bitmap[i] != 0xff)
		{
			for(j=0;j<8;j++)
			{
				unsigned char backup=bitmap[i];
				backup=backup>>j;
				backup&=0x1;
				if(!backup) break;
			}
			last_found_character=i; 
			bitmap [i] = bitmap [i] | power (2,j);			
			return (i*8+j)*PAGE_SIZE; 
		}
	}
	return 0xFFFFFFFF;
}


void physical_memory :: rdeallocate (unsigned int physical_address)
{
	unsigned char ander=~(power (2,physical_address % 8));
	bitmap [(physical_address / PAGE_SIZE)/8] &= ander; 
}

/* this function returns the offset in the character where it found a free frame
   the character where the free frame was found is updated on last_found_frame */
unsigned int physical_memory::search()
{
  int j;

  /* search for a free frame from last_found_character to end of bitmap */
  for(int i=last_found_character;i<BITMAP_SIZE;i++)
    {  

      /* check if any bit of this character of bitmap is 0 */
      if(bitmap[i]!=(unsigned char)(0xFF))
      {
      
      /* find what bit is zero - this algorithm is likely to get improved in future */
      for(j=0;j<8;j++)
        {
          unsigned char backup=bitmap[i];
          backup=backup>>j;
 	  backup&=0x1;
	  if(!backup) break;
        }
       last_found_character=i; 

       /* return because a free frame has been found */
       return j; 
      }
    }

  /* search for a free frame from start to last_found_character */
   for(int i=RESERVED_FRAMES/8;i<last_found_character;i++)
    {  

      /* check if any bit of this character of bitmap is 0 */
     if(bitmap[i]!=(unsigned char)(0xFF))
      {

      /* find what bit is zero - this algorithm is likely to get improved in future */
      for(j=0;j<8;j++)
        {
         unsigned char backup=bitmap[i];
	 backup=backup>>j;
	 backup&=0x1;
	 if(!backup) break;
        }
       last_found_character=i; 

       /* return because a free frame has been found */
       return j; 
      }
    }

  /* if the function has not returned any free frame, we need to swap some pages to disks */
  
}

/* this function returns the base address of a free frame - either from stack or from bitmap */
unsigned int physical_memory::allocate()
{
  unsigned int base_address;

  /* if stack is empty, we have to search the bitmap */
  if(stack_top==STACK_SIZE)
    {

     /* search for a free frame */
     unsigned int offset_into_character=search();
     bitmap [last_found_character] |= power (2,offset_into_character);

     /* find the base address of the frame from the absolute frame number */
     base_address=PAGE_SIZE*(last_found_character * 8 + offset_into_character);     
    } 
   else
    {

     /* take a frame from the stack */
     base_address=PAGE_SIZE*stack[stack_top];

     /* to complete the pop operation */
     stack_top++;

    } 

    /* return the base address of the frame */
    return base_address; 	
}

/* this function deallocates a frame and marks it as unused */
void physical_memory::deallocate(unsigned int base_address)
{

  /* if stack is not full, push the frame into stack */

  if(stack_top!=0)

    /* just push the frame on to stack */
    stack[--stack_top]=base_address/PAGE_SIZE;
  else
  {
    /* else update the bitmap */
    unsigned int byte_location=base_address/8;
    unsigned short int offset_into_byte=stack[stack_top]%8;
    bitmap[byte_location]&=power(2,offset_into_byte);     
  }
}

/**********************************************************************************

		LOGICAL MEMORY CLASS FUNCTIONS

***********************************************************************************/


/* this function is the constructor for the logical memory object */
void logical_memory :: initialize_memory (unsigned short int type)
{
	/* get a frame for our page directory */
	page_directory = (unsigned int*)(pm.rallocate ());

	/* mark all page tables as not present */
	for (int i=0;i<1024;i++)
		page_directory [i] = 2;

	if (type == USER_LOGICAL_MEMORY)
	{
		kdm_end_pointer = 0x40000000; //1GB
		dynamic_memory_start = 0x40000000;
		
		for (int i=0;i<8;i++)
			page_directory[i] = klm.page_directory [i];
		for (int i=8;i<1024;i++)
			page_directory [i] = 0x2;

		map (0xfffff000,pm.allocate (),0x7);

		page_directory [256] = (pm.rallocate ()) | 0x7;
		unsigned int *page_table = (unsigned int*)(page_directory [256] & 0xFFFFF000);
		for (int j=0;j<1024;j++)
			page_table[j]=0x2;

		

	}
	else
	{
		kdm_end_pointer = 0x1000000; //16 MB
		for (int i=4;i<8;i++)
		{
			page_directory [i] = (pm.rallocate ()) | 0x7;
			unsigned int *page_table = (unsigned int*)(page_directory [i] & 0xFFFFF000);
			for (int j=0;j<1024;j++)
			{
				page_table[j]=0x2;
			}
		}
		for (int i=8;i<1024;i++)
			page_directory [i] = 0x2;

		dynamic_memory_start = 0x1000000;
		
	}

}

void logical_memory :: load_page_directory ()
{
 /* load ptbr defined in start.s */
 load_ptbr ((unsigned int) (page_directory));
}

void logical_memory :: map (unsigned int virtual_address,unsigned int physical_address,unsigned short int attributes)
{

 /* find out the page directory entry */
 int page_directory_entry = (virtual_address >> 22) & 0x3FF;

 /* find out the page table entry */
 int page_table_entry = (virtual_address >> 12) & 0x3FF;

 /* pointer to the page table */
 unsigned int *page_table;


 /* if page table not present find a free frame and create the page_table */
 if ((page_directory [page_directory_entry] & 0x1) == 0)
 {
   /* allocate a physical frame for page table */
   page_table = (unsigned int*)(pm.rallocate ());


   /* fill in all the entries of the page table as not present */
   for (int i=0;i<1024;i++)
     page_table [i] = 0x2;

   /* put the page table address in that entry */
   page_directory [page_directory_entry] = (unsigned int) (page_table);

   /* mark the page table present bit */
   page_directory [page_directory_entry] = page_directory [page_directory_entry] | 0x7;
 }
 else
 {
   /* find out the page table address */
   page_table = (unsigned int*)(page_directory [page_directory_entry] & 0xFFFFF000);
 }

 page_table [page_table_entry] = (physical_address) | 0x7;

}

int logical_memory :: get_page_attributes (unsigned int virtual_address)
{
	/* find out the page directory entry */
	int page_directory_entry = (virtual_address >> 22) & 0x3FF;

	/* check if page table is present */
	if ((page_directory [page_directory_entry] & 0x1) == 0)
		return -1;

	/* find out the page table entry */
	int page_table_entry = (virtual_address >> 12) & 0x3FF;

	unsigned int *page_table = (unsigned int*)(page_directory[page_directory_entry] & 0xFFFFF000); 
	return (page_table [page_table_entry] & 0xFFF);
}

void check_heap (unsigned int virtual_address,unsigned int size)
{

  /* get the page begin address */
  unsigned int page_address = virtual_address & 0xFFFFF000;

  /* get the offset within page */
  unsigned int offset = virtual_address & 0x00000FFF;
  bool flag;

  /* loop variables */
  kdm_heap_entry *temp,*temp1;

  /* temporary heap entry */
  kdm_heap_entry *the;  

  /* address of the first element of the heap */
  kdm_heap_entry *heap_begin = 
	(kdm_heap_entry *) (page_address + PAGE_SIZE - sizeof (kdm_page_attribute) - sizeof(kdm_heap_entry));

  /* address of the page attribute that is within the page */
  kdm_page_attribute *page_attribute = 
	(kdm_page_attribute *) (page_address + PAGE_SIZE - sizeof (kdm_page_attribute));

  /* get the count of heap */  
  int heap_count = page_attribute -> count;

  /* address of the last element of the heap */
  kdm_heap_entry *heap_end = heap_begin - (heap_count - 1);

  /* if there is some element in heap */
  if (heap_count != 0)
  {
//  disp1.printf ("\ncheck heap = %x,%x,%x\n",heap_count,virtual_address,size);

    /* flag to mark whether any combining has been done */
    flag = 0;

    /* traverse through the elements of the heap */
    for (temp = heap_begin; temp >= heap_end ; temp --)
    {
      /* if we find a free block before the current block */
      if ((temp -> offset_in_page + temp -> size_of_block) == offset)
      {
	/* if we did not do any merging */
        if (!flag)
        {
          temp -> size_of_block += size;
          the = temp;
	  flag = 1;
	  offset = temp -> offset_in_page;
	  size = temp -> size_of_block;
	}

	/* if we did a merging before */
        else
	{
	  the -> size_of_block += temp -> size_of_block;
	  the -> offset_in_page = temp -> offset_in_page;
	  for (temp1 = temp;temp1>=heap_end;temp1--)
	    *temp1 = *(temp1-1);    	  
	  page_attribute -> vhp += sizeof (kdm_heap_entry);
	  page_attribute -> count -= 1;
        }
      }

      /* if we find a free block after the current block */
      else if ((offset + size) == temp -> offset_in_page)
      {

	/* if we did not do any merging */
        if (!flag)
        {
	  temp -> offset_in_page = offset;
	  temp -> size_of_block += size;                        
          the = temp;
	  flag = 1;
	  offset = temp -> offset_in_page;
	  size = temp -> size_of_block;
	}

	/* if we did a merging before */
	else
	{
	  the -> offset_in_page = temp -> offset_in_page;
	  the -> size_of_block += temp -> size_of_block;
	  for (temp1 = temp;temp1>=heap_end;temp1--)
	    *temp1 = *(temp1-1);    	  
	  page_attribute -> vhp += sizeof (kdm_heap_entry);
	  page_attribute -> count -= 1;
	}
      }
    }

    /* if no combining was done add a new free block to the end of heap */
    if (!flag)
    {
	heap_end --;
	heap_end -> offset_in_page = offset;
	heap_end -> size_of_block = size;
	page_attribute -> vhp += sizeof (kdm_heap_entry);
        page_attribute -> count ++;
    }
  }
  else
  {
    heap_end = heap_begin;
    heap_end -> offset_in_page = offset;
    heap_end -> size_of_block = size;
    page_attribute -> count = 1;
  }
}


void heapify (unsigned int virtual_address)
{
  /* get the page begin address */
  unsigned int page_address = virtual_address & 0xFFFFF000;

  /* address of the first element of the heap */
  kdm_heap_entry *heap_begin = 
	(kdm_heap_entry *) (page_address + PAGE_SIZE - sizeof (kdm_page_attribute) - sizeof(kdm_heap_entry));

  /* address of the page attribute that is within the page */
  kdm_page_attribute *page_attribute = 
	(kdm_page_attribute *) (page_address + PAGE_SIZE - sizeof (kdm_page_attribute));

  /* get the count of heap */  
  int heap_count = page_attribute -> count;

  unsigned short int i,j;
  kdm_heap_entry *parent,key;

  for (i=2;i<=heap_count;i++)
  {
    j = i/2;
    key = *(heap_begin - (i-1));
    parent = heap_begin - (j-1);
    while (j >= 1 && (parent -> size_of_block < key.size_of_block) )
    {
      *(heap_begin - (i-1)) = *(heap_begin - (j-1));
      i = j;
      j = i/2;
      parent = heap_begin - (j-1);
    }
    *(heap_begin - (i-1)) = key;
  }
}

/* function that searches any heap for a free block of particular size */

unsigned int logical_memory :: search_heap (unsigned int page_address,unsigned int size)
{
  if ((get_page_attributes (page_address) & 0x800) != 0) return 0xFFFFFFFF;
  page_address = page_address & 0xFFFFF000;
  struct kdm_page_attribute * kpa = (kdm_page_attribute*)(page_address + PAGE_SIZE - sizeof (kdm_page_attribute));

  /* address of the first element of the heap */
  kdm_heap_entry *heap_begin = 
	(kdm_heap_entry *) (page_address + PAGE_SIZE - sizeof (kdm_page_attribute) - sizeof(kdm_heap_entry));

  /* get the count of heap */  
  int heap_count = kpa -> count;

  /* address of the last element of the heap */
  kdm_heap_entry *heap_end = 
	heap_begin -(heap_count - 1);




 /* if there is any element in the heap then search this heap */
 if (heap_count > 0)
 {


   /* check the top element of the heap */
   if ((heap_begin -> size_of_block) >= size)
   {


     /* count the extra free space that is going to occur after allocation */
     int extra_free_space = (heap_begin -> size_of_block) - size;

     int start = heap_begin -> offset_in_page;

     /* do we need to add the remaining space to heap */
     if (extra_free_space > 0)
     {
       heap_begin -> offset_in_page = heap_begin -> offset_in_page + size;
       heap_begin -> size_of_block = extra_free_space;
       heapify (page_address);
     }
     else
     {
       for (int i=0;i<(heap_count-1);i++)
         *(heap_begin - i) = *(heap_begin - i - 1); 
       kpa-> count --;
       heapify (page_address);
     }
	
     /* decrease the virtual heap pointer because an allocation has taken place */
     kpa -> vhp -= 3;

     return (page_address | (start&0xfff));           
   }
  }
  
  /* cannot find a suitable block in heap */
  return 0xFFFFFFFF;

}

/////////////// IMPORTANT FUNCTION - KDMALLOC //////////////////////////////////

void* logical_memory :: kdmalloc (unsigned int size)
{

 int i,j;
 void *return_address;

// disp1.printf ("\nmalloc req = %x,",size);

/* if requested size is greater that 4086, this number is taken becuase at the worst case we may need 2 blocks in a heap combining with kdm_page_attribute we may need 9 bytes and atleast 1 byte free space after allocation in this block */

 if (size > (PAGE_SIZE-10))
 {
    if (((get_page_attributes (kdm_end_pointer) & 0x800) == 0) && ((get_page_attributes (kdm_end_pointer) & 0x1) != 0))
    {
	j = kdm_end_pointer & 0xFFFFF000;
	
            struct kdm_page_attribute * kpa = 
		(kdm_page_attribute*)(j + PAGE_SIZE - sizeof (kdm_page_attribute));

	    /* insert the new free block into the heap  */         
	    check_heap (kdm_end_pointer, (kpa -> vhp - (0xfff & kdm_end_pointer)));

	    /* construct the heap */       
	    heapify (kdm_end_pointer);
    }

    /* make kdm_end_pointer a 4 KB aligned address */
    if ((kdm_end_pointer & 0xFFF) != 0)
      kdm_end_pointer = (kdm_end_pointer & 0xFFFFF000) + 0x1000;

    /* the address to be returned */
    return_address = (void*)kdm_end_pointer;
    
    /* map all the complete pages required for allocation */
    for (i=0,j=kdm_end_pointer;i< (size/PAGE_SIZE) ;i++,j+=PAGE_SIZE)

	/* attribute is 0x803 - MSB of 12-bit attribute indicates that the page has no heap */
	map (j,pm.allocate (),0x803);

    /* if size is exact multiple of 4 KB */
    if (size % PAGE_SIZE == 0)
    {
      /* get kdm_end_pointer point to the start of next page */
      kdm_end_pointer = j;
    }

    /* if again we face with a problem that we need to allocate a complete frame because of insufficient free space left 	after allocation for creating a heap */
    else if(size % PAGE_SIZE > (PAGE_SIZE - 10))
    {
      /* attribute is 0x803 - MSB of 12-bit attribute indicates that the page has no heap */
      map (j,pm.allocate (),0x803);

      /* get kdm_end_pointer point to the start of next page */
      kdm_end_pointer = j + PAGE_SIZE;

    }
    else 
    {
      /* create a new frame with heap possible */
      map (j,pm.allocate (),0x7);

      /* point the kdm_end_pointer to the end of the allocated block */
      kdm_end_pointer = j + (size % PAGE_SIZE);

      /* make a pointer to the heap attribute */      
      struct kdm_page_attribute * kpa = (kdm_page_attribute*)(j + PAGE_SIZE - sizeof (kdm_page_attribute));

      /* initialize the value of heap attribute */      
      kpa -> vhp = PAGE_SIZE - sizeof (kdm_page_attribute) - sizeof (kdm_heap_entry);
      kpa -> count = 0;
    }

    /* return the address */
//    disp1.printf ("addr = %x,",return_address);
    return return_address;
 }

 
 j = kdm_end_pointer & 0xFFFFF000;

 if ((get_page_attributes (kdm_end_pointer) & 0x1) == 0)
 {


   /* search previous heaps */

   unsigned int start = ((kdm_end_pointer&0xFFFFF000)-0x1000); 
/*
   for (unsigned int i=start;i>=dynamic_memory_start;i-=0x1000)
   {
        unsigned int addr = search_heap (i,size);
        if (addr != 0xFFFFFFFF) return ((void*)addr);
   }
*/
   /* allocate a new block if no block is present */
   map (kdm_end_pointer,pm.allocate (),0x7);      
   return_address = (void*) kdm_end_pointer;

   /* make a pointer to the heap attribute */      
   struct kdm_page_attribute * kpa = (kdm_page_attribute*)(j + PAGE_SIZE - sizeof (kdm_page_attribute));

   /* initialize the value of heap attribute */      
   kpa -> vhp = PAGE_SIZE - sizeof (kdm_page_attribute) - sizeof (kdm_heap_entry);
   kpa -> count = 0;

   /* return the allocated block */
   kdm_end_pointer = kdm_end_pointer + size;
//    disp1.printf ("addr = %x,",return_address);
   return return_address;
 }


 struct kdm_page_attribute * kpa = (kdm_page_attribute*)(j + PAGE_SIZE - sizeof (kdm_page_attribute));

 if (((unsigned int)kdm_end_pointer + size) >= (j + kpa -> vhp - 3)) 
 {


   /* search previous heaps */
  unsigned int start = (kdm_end_pointer&0xFFFFF000); 
/*
   for (unsigned int i=start;i>=dynamic_memory_start;i-=0x1000)
   {
     unsigned int addr = search_heap (i,size);
     if (addr != 0xFFFFFFFF) return ((void*)addr);
   }

   j = kdm_end_pointer & 0xFFFFF000;
	

	check_heap (kdm_end_pointer, (kpa -> vhp - (0xfff & kdm_end_pointer)));

	heapify (kdm_end_pointer);
*/ 
    /* make kdm_end_pointer a 4 KB aligned address */
    kdm_end_pointer = (kdm_end_pointer & 0xFFFFF000) + 0x1000;

    /* create a new frame with heap possible */
    map (kdm_end_pointer,pm.allocate (),0x7);

    /* the address to be returned */
    return_address = (void*)kdm_end_pointer;

    /* increment the kdm_end_pointer */   
    kdm_end_pointer += size;

    /* make a pointer to the heap attribute */      
    j = kdm_end_pointer & 0xFFFFF000;
    struct kdm_page_attribute * kpa = (kdm_page_attribute*)(j + PAGE_SIZE - sizeof (kdm_page_attribute));

    /* initialize the value of heap attribute */      
    kpa -> vhp = PAGE_SIZE - sizeof (kdm_page_attribute) - sizeof (kdm_heap_entry);
    kpa -> count = 0;

    /* return the address */
//    disp1.printf ("addr = %x,",return_address);
    return return_address;        
 }
 else
 {
    /* the address to be returned */
    return_address = (void*)kdm_end_pointer;    
    j = kdm_end_pointer & 0xFFFFF000;

    /* increment the kdm_end_pointer */
    kdm_end_pointer += size;

    /* make a pointer to the heap attribute */      
    struct kdm_page_attribute * kpa = (kdm_page_attribute*)(j + PAGE_SIZE - sizeof (kdm_page_attribute));

    /* initialize the value of heap attribute */      
    kpa -> vhp -= sizeof (kdm_heap_entry);

    /* return the address */
//    disp1.printf ("addr = %x,",return_address);
    return return_address;        
 }
}

void logical_memory :: print_heap_details (unsigned int addr)
{
  unsigned int j = addr & 0xFFFFF000;
  struct kdm_page_attribute * kpa = (kdm_page_attribute*)(j + PAGE_SIZE - sizeof (kdm_page_attribute));
  struct kdm_heap_entry *khp = (kdm_heap_entry*)(j + PAGE_SIZE - sizeof (kdm_page_attribute) - sizeof (kdm_heap_entry));
  disp1.putstring ("\nheap count = ");
  disp1.putint_hexadecimal (kpa->count);
  disp1.putstring ("\nvhp = ");
  disp1.putint_hexadecimal (kpa->vhp);
  disp1.putstring ("\nkdm end pointer = ");
  disp1.putint_hexadecimal (kdm_end_pointer);
  for (int i=0;i<kpa->count;i++)
  {
    disp1.putstring ("\n element ");
    disp1.putint_decimal (i+1);
    disp1.putstring (": (");
    disp1.putint_hexadecimal (khp->offset_in_page);
    disp1.putstring (",");
    disp1.putint_hexadecimal (khp->size_of_block);
    disp1.putstring (") \n");    
    khp --;
  }
}

void logical_memory :: set_page_attribute (unsigned int virtual_address,unsigned short int attribute)
{
	/* find out the page directory entry */
	int page_directory_entry = (virtual_address >> 22) & 0x3FF;

	/* check if page table is present */
	if ((page_directory [page_directory_entry] & 0x1) == 0)
		return ;

	/* find out the page table entry */
	int page_table_entry = (virtual_address >> 12) & 0x3FF;

	/* pointer to the page table */
	unsigned int *page_table = (unsigned int*)(((unsigned int)page_directory [page_directory_entry]) & 0xFFFFF000);

	/* modify page attributes */
	page_table [page_table_entry] = page_table [page_table_entry] & (0xFFF & attribute);
}

unsigned int logical_memory :: get_physical_address (unsigned int virtual_address)
{
	/* find out the page directory entry */
	int page_directory_entry = (virtual_address >> 22) & 0x3FF;

	/* check if page table is present */
	if ((page_directory [page_directory_entry] & 0x1) == 0)
		return 0xFFFFFFFF;

	/* find out the page table entry */
	int page_table_entry = (virtual_address >> 12) & 0x3FF;

	/* pointer to the page table */
	unsigned int *page_table = (unsigned int*)(((unsigned int)page_directory [page_directory_entry]) & 0xFFFFF000);

	/* modify page attributes */
	return page_table [page_table_entry] & 0xFFFFF000;
}

/////////////// IMPORTANT FUNCTION - KDFREE //////////////////////////////////

/* logical memory freeing funtion */
void logical_memory :: kdfree (void *address,unsigned int size)
{

	unsigned int i=(unsigned int)(address);
  
	/* free all the complete pages that were allocated */
	for (;i < size / PAGE_SIZE; i+=4096)
	{
		/* get the actual physical address of the page */
		unsigned int physical_address = get_physical_address (i);

		/* physically deallocate the frame */
		pm.deallocate (physical_address);

		/* mark the page present as false */
		set_page_attribute (i,0x800);
	}
    
  /* check if a complete page was allocated for the rest of the block */
  if (size % PAGE_SIZE > (PAGE_SIZE - 10))
  {
    /* get the actual physical address of the page */
    unsigned int physical_address = get_physical_address (i);

    /* physically deallocate the frame */
    pm.deallocate (physical_address);

    /* mark the page present as false */
    set_page_attribute (i,0x800);
  }    
  else
  {

    /* if there is a chance to reverse the kdm end pointer */
    if (((unsigned int)address + size) == (unsigned int)kdm_end_pointer)
    {
      kdm_end_pointer -= size;

      /* change the value of virtual heap pointer */
      struct kdm_page_attribute * kpa = 
	(kdm_page_attribute *)(((unsigned int)address&0xFFFFF000) + PAGE_SIZE - sizeof (kdm_page_attribute));
      kpa -> vhp += sizeof (kdm_heap_entry);
    }
    else
    {
      /* add the entry and preserve the heap structure */
      check_heap ((unsigned int)address,size);
      heapify ((unsigned int)address);
    }  
  }
}

void* operator new (unsigned int size)
{
	return klm.kdmalloc(size);
}

void logical_memory :: mmap (unsigned int source_virtual_address,logical_memory *destination,unsigned int destination_virtual_address)
{
	unsigned int *source_page_table_address,tmp,*destination_page_table_address;
	unsigned int *ksource_pointer,*kdestination_pointer;
	
	/* what if source page table is not there */
	source_page_table_address = page_directory + ((source_virtual_address >> 22)&0x3ff);
	destination_page_table_address = destination -> page_directory + ((destination_virtual_address >> 22)&0x3ff);
	
	ksource_pointer = (unsigned int*)klm.kdmalloc (PAGE_SIZE);
	kdestination_pointer = (unsigned int*)klm.kdmalloc (PAGE_SIZE);
	
	klm.kdfree (ksource_pointer,PAGE_SIZE);
	klm.kdfree (kdestination_pointer,PAGE_SIZE);
	
	klm.map ((unsigned int)ksource_pointer,(unsigned int)source_page_table_address,0x03);
	klm.map ((unsigned int)kdestination_pointer,(unsigned int)destination_page_table_address,0x03);
	
	kdestination_pointer [((destination_virtual_address >> 12)&0x3FF)] = 
			ksource_pointer [((source_virtual_address >> 12) & 0x3ff)];

	klm.set_page_attribute ((unsigned int)ksource_pointer,0x0);
	klm.set_page_attribute ((unsigned int)kdestination_pointer,0x0);
}

unsigned int * logical_memory :: get_page_directory_address ()
{
	return page_directory;
}
