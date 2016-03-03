#include <filesystem.h>
#include <video.h>
#include <idt.h>
#include <memory.h>
#include <common.h>
#include <storage.h>
#include <filemanager.h>
extern display disp1;
extern logical_memory klm;
extern file_table *ft_head;
extern unsigned int cd_lba;
extern char *cd_name;

iso9660::iso9660(drive *d)
{
  

  unsigned char bf[2048];
  int i,count;
  drv = d;
  drv->pioread(64,4,bf);
  pr.vdt=bf[0];
  pr.si[0]=bf[1];
  pr.si[1]=bf[2];
  pr.si[2]=bf[3];
  pr.si[3]=bf[4];
  pr.si[4]=bf[5];
  if(pr.si[0]=='C' && pr.si[1]=='D' && pr.si[2]=='0' && pr.si[3]=='0' && pr.si[4]=='1' && pr.vdt==1)
    disp1.printf("\nIso9660 file system detected\n");
  else
    {
      disp1.printf("\nUnknown file system\n");
	return;
    }
  //disp1.printf("\nsystem identifier ");
  for(i=0;i<32;++i)
    {
      //disp1.printf("%c",bf[8+i]);
      pr.sid[i]=bf[8+i];
    }
  //disp1.printf("\nvolume identifier ");
  
  for(i=0;i<32;++i)
    {
      //disp1.printf("%c",bf[40+i]);
      pr.vid[i]=bf[40+i];
    }
  //disp1.printf("\nvolume space size ");
  pr.vss=return_int (bf,84,1);
  //disp1.printf("%x",pr.vss);
  
  //disp1.printf("\nvolume set size  ");
  
  pr.vsts=two_return_int (bf,122,1);
  //disp1.printf("%x",pr.vsts);
  
  
  
  //disp1.printf("\nvolume seq number ");
  
  pr.vsn=two_return_int (bf,126,1);
  //disp1.printf("%x",pr.vsn);
  
  //disp1.printf("\nlogical block size ");
  
  pr.lbs=two_return_int (bf,130,1);
  //disp1.printf(" %x",pr.lbs);
  
  //disp1.printf("\npath table size ");

  pr.pts=return_int (bf,136,1);
  //disp1.printf("%x",pr.pts);
  
  //disp1.printf("\nlocation of l path table ");
  
  pr.lptb=return_int (bf,140,0);
  //disp1.printf("%x",pr.lptb);

  //disp1.printf("\nlocation of m path table ");
  
  pr.mptb=return_int (bf,148,1);
  //disp1.printf("%x",pr.mptb);
  
  //disp1.printf("\nroot directory record ");
  
  for(i=0;i<34;++i)
    {
      //disp1.printf("%x",bf[156+i]);
      pr.rdr[i]=bf[156+i];
    }
  load_rdr(0,pr.rdr);
  //disp1.printf("\nApplication identifier ");
  
  for(i=0;i<128;++i)
    {
//disp1.printf("%c",bf[574+i]);
      pr.apid[i]=bf[574+i];
    }
  //disp1.printf("\ndate and time of volume creation ");
  
for(i=0;i<16;++i)
  {
    //disp1.printf("%c",bf[813+i]);
    pr.vdnt[i]=bf[813+i];
  }
//disp1.printf("\ndate and time of volume modification ");
 
 for(i=0;i<16;++i)
   {
     //disp1.printf("%c",bf[830+i]);
     pr.mdnt[i]=bf[830+i];
   }
 //disp1.printf("\nEf	fective date and time of volume ");
 
 for(i=0;i<16;++i)
   {
     //disp1.printf("%c",bf[864+i]);
     pr.ednt[i]=bf[864+i];
   }
 
////////////////////////////////////////////////////////////////////////////////////////
 
 
}

void iso9660::load_rdr(int offset,unsigned char *r_array)
{

  
  //disp1.printf("\nLength directory record ");
  rd.len_dir=r_array[0+offset];
  //disp1.printf("%x",r_array[0+offset]);
  
  
  //disp1.printf("\nExtented attribute record length ");
  rd.ext_attr_len=r_array[1+offset];
  //disp1.printf("%x",r_array[1+offset]);

  
  //disp1.printf("\nlocation of extent ");
  rd.loc_ext=return_int (r_array,6+offset,1);
  //disp1.printf("%x",rd.loc_ext);

  //disp1.printf("\nData length ");
rd.data_len=return_int (r_array,14+offset,1);
//disp1.printf("%x",rd.data_len);
 
//disp1.printf("\nRecording date and time ");
 for(int i=0;i<7;++i)
   {
     rd.date_time[i]=r_array[18+offset+i];
     //disp1.printf("%c",r_array[18+offset+i]);
   }
 //disp1.printf("\nFile flag ");
 rd.file_flag=r_array[25+offset];
 //disp1.printf("%x",r_array[25+offset]);
 

 //disp1.printf("\nFile unit size ");
 rd.file_unit_size=r_array[26+offset];
 //disp1.printf("%x",r_array[26+offset]);
 
 //disp1.printf("\niInterleave Gap size ");
 rd.int_gap_size=r_array[27+offset];
 //disp1.printf("%x",r_array[27+offset]);

 //disp1.printf("\nVolume seq number ");
 rd.vol_seq_num=two_return_int (r_array,30+offset,1);
 //disp1.printf("%x",rd.vol_seq_num);
 
 //disp1.printf("\nLength of file identifier ");
 rd.len_file_id=r_array[32+offset];
 //disp1.printf("%x",r_array[32+offset]);
 
 
 //disp1.printf("\nFile identifier ");
 for(int i=0;i<rd.len_file_id;++i)
   {
     rd.file_id[i]=r_array[33+offset+i];
     //disp1.printf("%c",r_array[33+offset+i]);
   }
 
}

void iso9660::load_ptb(int offset,unsigned char *r_array)
{
  

  
  pt.len_di=r_array[0+offset];
  //disp1.printf("\nLength of Directory identifier ");
  //disp1.printf("%x",pt.len_di);
  
  pt.ext_attr_len=r_array[1+offset];
  //disp1.printf("\nExtented attribute record ");
  //disp1.printf("%x",pt.ext_attr_len);
  
  pt.loc_ext=return_int (r_array,2+offset,0);
  //disp1.printf("\nLocation of extent ");
  //disp1.printf("%x",pt.loc_ext);
  
  pt.par_dir_no=two_return_int (r_array,6+offset,0);
  //disp1.printf("\nParent Diectory no  ");
  //disp1.printf("%x",pt.par_dir_no);
  
  //disp1.printf("\nDirectory identifier ");
  for(int i=0;i<pt.len_di;++i)
    {
      pt.dir_id[i]=r_array[8+offset+i];
      //  disp1.printf("%c",pt.dir_id[i]);
    }
  
  //disp1.printf("\nPadding field ");
  for(int i=0;i<pt.len_di;++i)
    {
      pt.pad_fld[i]=r_array[8+offset+pt.len_di+i];
      //disp1.printf("%c",pt.pad_fld[i]);
      
    }
  
  
}
int iso9660::fopen(char *path,char *mod)
{
  
  int flag,bp,prv_fldr=1,f_cnt,g,st=0,i=0,j,file_flag=0,folder_flag=0,k,count;
  unsigned char tname[30],*bf,*bf2;

  short int file_location,parent;
  if((pr.pts%512)==0)
    count=pr.pts/512;
  else
    count=(pr.pts/512)+1;
  bf=(unsigned char*) (klm.kdmalloc (count*512));
  
  
  drv->pioread(pr.lptb*4,count,bf);
   int off=0;
  path++;
 
  while(path[i]!='\0')
    {
      load_ptb(0,bf);
      off=0;
      bp=0;
      flag=0;
      j=0;
      if(i==0)
	file_location=pt.loc_ext;

      while(path[i]!='/' && path[i]!='\0')
	{
	  tname[j++]=path[i++];
	}
      tname[j]='\0';
      if(path[i]=='/')
	{
	  i++;
	  while(off<pr.pts)
	    {
	      if(pt.len_di%2==0)	
		{
		  off=off+8+pt.len_di;
		  if(off!=pr.pts)
		    {	
		      load_ptb(off,bf);
		      bp++;
		    }
		  if(prv_fldr!=pt.par_dir_no)
		    {
		 
		      flag=0;
		      continue;
		    }
		 
		  for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];++g);
		  if((off==pr.pts && pt.len_di!=g)||(g==pt.len_di && parent==pt.par_dir_no && tname[g]!='\0'))
		    {
		      disp1.printf("\nInvalid Directory %s",tname);
		      return 0;
		    }
		  
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		      
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      flag=1;
		      //disp1.printf("\nParent Directory of %s is %x",tname,parent);
		      break;
		    }
		}
	      
	      else
		{
		  off=off+8+pt.len_di+1;
		  if(off!=pr.pts)
		    {
		      load_ptb(off,bf);
		      bp++;
		    }

		  if(prv_fldr!=pt.par_dir_no)
		    {
		     
		      flag=0;
		      continue;
		    }

		 
		  for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];++g);
		  if((off==pr.pts && g!=pt.len_di)||(g==pt.len_di && parent==pt.par_dir_no && tname[g]!='\0'))
		    {
		      disp1.printf("\nInvalid Directory %s",tname);
		      return 0;
		    }
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		    
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      //disp1.printf("\nParent Directory of %s is %x",tname,parent);
		      flag=1;
		      break;
		    }
		}
	    }
	  if(flag==0)
	    {
	    
	      disp1.printf("\nCannot find the directory %s",tname);
	      return 0;
	    }
	}	  
      
      else if(mod[0]=='r' && mod[1]=='\0')
	{
	  
	  //disp1.printf("\nNonesense location %x %x",file_location,pr.lbs);

	  if((pr.lbs%512)==0)
	    count=pr.lbs/512;
	  else
	    count=(pr.lbs/512)+1;
	  bf2=(unsigned char*) (klm.kdmalloc (count*512));
	  
	  drv->pioread(file_location*4,count,bf2);
	  off=0;
	  flag=0;
	  load_rdr(off,bf2);
	  int h;
	  while( rd.len_dir !=0)
	    {
	      if(rd.file_flag==0x00)
		{
		  for(h=0;tname[h]!='\0' && tname[h]==rd.file_id[h];++h);
		  if(h==rd.len_file_id && tname[h]=='\0')
		    {
		      
		      flag=1;
		      break;
		    }
		  
		}
	      off=off+rd.len_dir;
	      load_rdr(off,bf2);
	      
	    }	  
	  if(flag==0)
	    {
	      disp1.printf("\nFile Not found ");
	      return 0;
	    }
	  else
	    {
/*	      //disp1.printf("\nFile found %s and location %x and size %x",tname,rd.loc_ext,rd.data_len);
	    if((rd.data_len%512)==0)
	      count=rd.data_len/512;
	    else
	      count=(rd.data_len/512)+1;
	    bf=(unsigned char*) (klm.kdmalloc (count*512));
	    
	    drv->pioread(rd.loc_ext*4,count,bf);
            file_begin=bf;
	    file_curr=bf;*/
	    extern file_table *ft_ptr;
	    extern int rid;
	    int cal;
	    cal=rid;
	    cal+=2;
	    if(cal%6==0)
             cal+=2;
            rid=cal;
 
	    ft_ptr->next=(file_table *) (klm.kdmalloc (sizeof(file_table)));
	    ft_ptr->next->fid=rid;
            ft_ptr->next->fentry_lba=0;
            ft_ptr->next->fentry_offset=0;
            ft_ptr->next->file_start_clstr_no=rd.loc_ext*4;
            ft_ptr->next->file_size=rd.data_len;
	    ft_ptr->next->fptr=0;
            ft_ptr->next->partition=this;
            ft_ptr->next->pid=0;
            ft_ptr->next->next=0;
            ft_ptr=ft_ptr->next;
            klm.kdfree(&bf2,sizeof(count*512));
	    return rid;
	      
	    }
	}
	else
	{
	disp1.printf("\nInvalid Mode Specified");
	return 0;
	}      
    }			
  
}

int iso9660::end_file(unsigned char *p)
{
  
  if(p[0]==0x00 && p[1]==0x00 && p[2]==0x00 && p[3]==0x00 && p[4]==0x00 && p[5]==0x00 && p[6]==0x00 && p[7]==0x00 && p[8]==0x00 && p[9]==0x00 && p[10]==0x00)
    return 1;
  else
    return 0;
  
}

int iso9660::fseek(file_table *temp /*cange*/,int bytes,int pos)
{

    //file_table *temp; 


    int count,t;

 /*   temp=ft_head->next;
    while(temp->fid!=f)
     {
	temp=temp->next;
     }
*/
    if(pos==0 && temp->file_size >= bytes)
     {
      temp->fptr=0;
      temp->fptr=temp->fptr+bytes;
      }
    else if(pos==0 && bytes<0)
     {
      temp->fptr=0;
      return temp->fptr;
      }
   else if(pos==1)
     {
        if(temp->fptr+bytes >temp->file_size)
        return temp->fptr=temp->file_size;
        if(temp->fptr + bytes <0)
	return temp->fptr=0;
	temp->fptr=temp->fptr+bytes;
  	return temp->fptr;
     }

    else if(pos==2 && bytes < 0)
     {
      temp->fptr=temp->file_size;
      temp->fptr=temp->fptr+bytes;
      return temp->fptr;
     }
    else if(pos==2 && bytes > 0)
     {
      temp->fptr=temp->file_size;
        return temp->fptr;
     }

  
  
}
unsigned int iso9660::fread(file_table *temp,unsigned char **buff,unsigned int bytes)
{
    

    int count,t;
  
    /*temp=ft_head->next;
    while(temp->fid!=f)
     {
	temp=temp->next;
     }*/
    if(temp->fptr >= temp->file_size)
     temp->fptr=0;
    t=bytes;
    bytes=temp->fptr+bytes;
    if(bytes>(temp->file_size))
     {
      t=temp->file_size - temp->fptr;
      bytes=(temp->file_size);
     }

    //disp1.printf("\nFile found and location %x ",temp->fptr);
    if((bytes%512)==0)
      count=bytes/512;
    else
      count=bytes/512+1;
   unsigned char *bf=(unsigned char*) (klm.kdmalloc (count*512));

     drv->pioread((temp->file_start_clstr_no),count,bf);
     //for(int i=0;i<t;++i)     
      //buff[i]=bf[temp->fptr+i];
     bf=bf+temp->fptr;
    *buff=bf;           /////change
     temp->fptr=bytes; 
      // disp1.printf("\n new location %x ",temp->fptr);
      
      return t;
}

bool iso9660::fclose(file_table *temp2) //change	
{
    file_table *temp,*temp1;
    int count,t;

  
/////////////change     

    temp=ft_head;
    if(temp->next!=0 && temp!=0)
    {
    //disp1.printf("\nhead address %x",ft_head->next); 
      
      while(temp->next!=temp2 && temp->next!=0)
       {
       	temp=temp->next;
       }
      if(temp!=0)
       {
        temp->next=temp2->next;
        klm.kdfree(&temp2,sizeof(file_table));
        disp1.printf("\nSuccessfully closed the file");
	return 1; 
       }
      else
        {
          disp1.printf("\nFile not found in the file table");
	  return 0;
        }

     }
   else
     {
      disp1.printf("\nNo file table is created");
     return 0;
     }
   
}

bool iso9660::flist(char *path)
{
  int flag,bp,prv_fldr=1,f_cnt,g,st=0,i=0,j,file_flag=0,folder_flag=0,k,count;
  unsigned char tname[30],*bf,*bf2;
  short int file_location,parent;
  if((pr.pts%512)==0)
    count=pr.pts/512;
  else
    count=(pr.pts/512)+1;
  bf=(unsigned char*) (klm.kdmalloc (count*512));
  drv->pioread(pr.lptb*4,count,bf);
  int off=0;
load_rdr(0,pr.rdr);
  
  if(path[0]!='\0' && path[0]=='/')
  {
      path++;
  }  
while(path[i]!='\0')
    {
    
      load_ptb(0,bf);
      off=0;
      bp=0;
      flag=0;
      j=0;
      if(i==0)
	file_location=pt.loc_ext;
      
      
      while(path[i]!='/' && path[i]!='\0')
	{
	  tname[j++]=path[i++];
	}
      tname[j]='\0';
      if(path[i]=='/' || path[i]=='\0')
	{
	  if(path[i]!='\0')
	  i++;
	  while(off<pr.pts)
	    {
	      if(pt.len_di%2==0)	
		{
		  off=off+8+pt.len_di;
		  if(off!=pr.pts)
		    {	
		      load_ptb(off,bf);
		      bp++;
		    }
		  if(prv_fldr!=pt.par_dir_no)
		    {
		      
		      flag=0;
		      continue;
		    }
		  
		  for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];++g);
		  if((off==pr.pts && pt.len_di!=g)||(g==pt.len_di && parent==pt.par_dir_no && tname[g]!='\0'))
		    {
		      disp1.printf("\nInvalid Directory %s",tname);
		      return 0;
		    }
		  
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		      
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      flag=1;
		      //disp1.printf("\nParent Directory of %s is %x",tname,parent);
		      break;
		    }
		}
	      
	      else
		{
		  off=off+8+pt.len_di+1;
		  if(off!=pr.pts)
		    {
		      load_ptb(off,bf);
		      bp++;
		    }
		  
		  if(prv_fldr!=pt.par_dir_no)
		    {
		      
		      flag=0;
		      continue;
		    }
		  
		  
		  for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];++g);
		  if((off==pr.pts && g!=pt.len_di)||(g==pt.len_di && parent==pt.par_dir_no && tname[g]!='\0'))
		    {
		      disp1.printf("\nInvalid Directory %s",tname);
		      return 0;
		    }
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		      
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      //disp1.printf("\nParent Directory of %s is %x",tname,parent);
		      flag=1;
		      break;
		    }
		}
	    }
	  if(flag==0)
	    {
	      
	      disp1.printf("\nCannot find the directory %s",tname);
	      return 0;
	    }
		
	}
    }
  	  
  //disp1.printf("\nNonesense location %x %x",file_location,pr.lbs);

  //disp1.printf("\n");
  
  if(path[0]=='\0' && *(path-1)=='/')
    {
  
      file_location=rd.loc_ext;
    
      
    }
  if((pr.lbs%512)==0)
    count=pr.lbs/512;
  else
    count=(pr.lbs/512)+1;
  bf2=(unsigned char*) (klm.kdmalloc (count*512));
  
  drv->pioread(file_location*4,count,bf2);
  off=0;
  flag=0;
  load_rdr(off,bf2);
  off=off+rd.len_dir;
  load_rdr(off,bf2);
  off=off+rd.len_dir;
  load_rdr(off,bf2);
  int h;
  while(rd.len_dir !=0)
    {
	if(rd.file_flag==0x00)
	   disp1.setattrib(0x02);
        else
	   disp1.setattrib(0x01);

      for(h=0;h<rd.len_file_id;++h)
	disp1.printf("%c",rd.file_id[h]);
      disp1.printf("  ");	
      off=off+rd.len_dir;
      load_rdr(off,bf2);
      
    }
  disp1.printf("\n");	  
  disp1.setattrib(0x0f);

  return 1;
}	

bool iso9660::move_to_dir(char *path)
{
  
  int flag,bp,prv_fldr=1,f_cnt,g,st=0,i=0,j,file_flag=0,folder_flag=0,k,count;
  unsigned char tname[30],*bf,*bf2;
  short int file_location,parent;
  if((pr.pts%512)==0)
    count=pr.pts/512;
  else
    count=(pr.pts/512)+1;
  bf=(unsigned char*) (klm.kdmalloc (count*512));
  load_rdr(0,pr.rdr);
  
  drv->pioread(pr.lptb*4,count,bf);
  int off=0;
  if(path[0]!='\0' && path[0]=='/')
  {
      path++;
  }  
  while(path[i]!='\0')
    {
      load_ptb(0,bf);
      off=0;
      bp=0;
      flag=0;
      j=0;
      if(i==0)
	file_location=pt.loc_ext;
      
      
      while(path[i]!='/' && path[i]!='\0')
	{
	  tname[j++]=path[i++];
	}
      tname[j]='\0';
      if(path[i]=='/' || path[i]=='\0')
	{
	  if(path[i]!='\0')
	    i++;
	  
	  while(off<pr.pts)
	    {
	      if(pt.len_di%2==0)	
		{
		  off=off+8+pt.len_di;
		  if(off!=pr.pts)
		    {	
		      load_ptb(off,bf);
		      bp++;
		    }
		  if(prv_fldr!=pt.par_dir_no)
		    {
		      
		      flag=0;
		      continue;
		    }
		  
		   for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];++g);
		  //disp1.printf("\n%c %x",tname[g],g);
		 
		  if((off==pr.pts && pt.len_di!=g)||(g==pt.len_di && parent==pt.par_dir_no && tname[g]!='\0'))
		    {
		      disp1.printf("\nInvalid Directory %s",tname);
		      return 0;
		    }
		  
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		      
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      flag=1;
		      //disp1.printf("\nParent Directory of %s is %x",tname,parent);
		      break;
		    }
		}
	      
	      else
		{
		  off=off+8+pt.len_di+1;
		  if(off!=pr.pts)
		    {
		      load_ptb(off,bf);
		      bp++;
		    }
		  
		  if(prv_fldr!=pt.par_dir_no)
		    {
		      
		      flag=0;
		      continue;
		    }
		
		  for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];g++);
		  // disp1.printf("\n%c %x",tname[g],g);
		    
		  if((off==pr.pts && g!=pt.len_di)||(g==pt.len_di && parent==pt.par_dir_no && tname[g]!='\0'))
		    {
		      disp1.printf("\nInvalid Directory %s",tname);
		      return 0;
		    }
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		      
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      //disp1.printf("\nParent Directory of %s is %x",tname,parent);
		      flag=1;
		      break;
		    }
		}
	    }
	  if(flag==0)
	    {
	      
	      disp1.printf("\nCannot find the directory %s",tname);
	      return 0;
	    }
	  
	}
    }
  
  //disp1.printf("\nNonesense location %x %x",file_location,pr.lbs);

//  disp1.printf("\n");
  
  if(path[0]=='\0' && *(path-1)=='/' )
    {
      
      file_location=rd.loc_ext;
      
    }

  //  cdfs_dir_addrs=file_location;
 // cd_name=path;
  cd_lba=file_location;
  //  disp1.printf("\nyou successfully moved to %s\n",path);
  return 1;
}

bool iso9660::fcd(char *path)
{

  return move_to_dir(path);
  
}
bool iso9660::fs_search(char *path,unsigned char file)
{
  int flag,bp,prv_fldr=1,f_cnt,g,st=0,i=0,j,file_flag=0,folder_flag=0,k,count;
  unsigned char tname[30],*bf,*bf2;

  short int file_location,parent;
  if((pr.pts%512)==0)
    count=pr.pts/512;
  else
    count=(pr.pts/512)+1;
  bf=(unsigned char*) (klm.kdmalloc (count*512));
  
  load_rdr(0,pr.rdr);
  drv->pioread(pr.lptb*4,count,bf);
   int off=0;
   //   disp1.printf("%s\n",path);
   if(path[0]!='\0' && path[0]=='/')
     path++;
   
   while(path[i]!='\0')
    {
      
      load_ptb(0,bf);
      off=0;
      bp=0;
      flag=0;
      j=0;
      if(i==0)
	file_location=pt.loc_ext;

      while(path[i]!='/' && path[i]!='\0')
	{
	  tname[j++]=path[i++];
	}
      tname[j]='\0';
      if(path[i]=='/')
	{
		
	  i++;
	  while(off<pr.pts)
	    {
	      if(pt.len_di%2==0)	
		{
		  off=off+8+pt.len_di;
		  if(off!=pr.pts)
		    {	
		      load_ptb(off,bf);
		      bp++;
		    }
		  if(prv_fldr!=pt.par_dir_no)
		    {
		 
		      flag=0;
		      continue;
		    }
		 
		  for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];++g);
		  if((off==pr.pts && pt.len_di!=g)||(g==pt.len_di && parent==pt.par_dir_no &&  tname[g]!='\0'))
		    {
		     
      
		      return 0;
		    }
		  
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		      
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      flag=1;
		      break;
		    }
		}
	      
	      else
		{
		  off=off+8+pt.len_di+1;
		  if(off!=pr.pts)
		    {
		      load_ptb(off,bf);
		      bp++;
		    }

		  if(prv_fldr!=pt.par_dir_no)
		    {
		     
		      flag=0;
		      continue;
		    }

		 
		  for(g=0;tname[g]!='\0' && tname[g]==pt.dir_id[g];++g);
		  if((off==pr.pts && g!=pt.len_di)||(g==pt.len_di && parent==pt.par_dir_no&& tname[g]!='\0'))
		    {
		      // disp1.printf("%s off=%x pr.pts=%x pt.len_di=%x g=%x parent=%x pt.par_dir_n=%x \n",tname,off,pr.pts,pt.len_di,g,parent,pt.par_dir_no);
		      return 0;
		    }
		  else if(g==pt.len_di && tname[g]=='\0')
		    {
		    
		      file_location=pt.loc_ext;
		      parent=pt.par_dir_no;
		      prv_fldr=bp+1;
		      flag=1;
		      break;
		    }
		}
	    }
	  if(flag==0)
	    {
	 
	      return 0;
	    }
	}	  
      
      else 
	{
	  
	  //disp1.printf("\nNonesense location %x %x %s",file_location,pr.lbs,tname);
	  
	

	  if((pr.lbs%512)==0)
	    count=pr.lbs/512;
	  else
	    count=(pr.lbs/512)+1;
	  bf2=(unsigned char*) (klm.kdmalloc (count*512));
	  
	  drv->pioread(file_location*4,count,bf2);
	  off=0;
	  flag=0;
	  load_rdr(off,bf2);
	  int h;
	 
	  while( rd.len_dir !=0)
	    {
	 
	      if(rd.file_flag==0x00 ||rd.file_flag==0x02 )
		{
		  for(h=0;tname[h]!='\0' && tname[h]==rd.file_id[h];++h);
		  if(h==rd.len_file_id && tname[h]=='\0')
		    {
		      
		      flag=1;
		      break;
		    }
		  
		}
	      off=off+rd.len_dir;
	      load_rdr(off,bf2);
	      
	    }	  
	  if(flag==0)
	    {

	      return 0;
	    }
	  else
	    {
	      
	    return 1;
	      
	    }
	}
    }

  if(path[0]=='\0' || *(path-1)=='/' )
    {
      
      file_location=rd.loc_ext;
      //disp1.printf("ayyooooo %x\n",file_location);
      return 1;
    }
  
  
}
