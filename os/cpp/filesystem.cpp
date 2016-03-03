#include<filesystem.h>
#include<memory.h>
#include<video.h>

extern logical_memory klm;
extern display disp1;
extern unsigned int cd_lba;
extern unsigned int rid,wid,rwid;
extern file_table *ft_ptr,*ft_head;
char dot=0;


unsigned int fat32::fat_lookup(unsigned int clstr_no)
{
	unsigned int fat_sector,offset;
	unsigned char *fat; 
	char count;
	fat=(unsigned char *)klm.kdmalloc(512);
	fat_sector=clstr_no>>7;
	fat_sector&=0x01FFFFFF;
	fat_sector+=fat_begin_lba;
	offset=clstr_no&0x7F;
	drv->pioread(fat_sector,1,fat);
	clstr_no=0;
	for(count=3;count>=0;count--)
	{
		 clstr_no=clstr_no<<8;
		 clstr_no|=*(fat+count+4*offset);
	}
	clstr_no&=0x0FFFFFFF;
	if(clstr_no>=0x0FFFFFF8)
	return(0);
	else
        return(clstr_no);
}


bool fat32::fs_search(char *name,unsigned char file)
{
	unsigned char name_offset,history,*dir_name,*clstr,*temp,flag=1,*fat;
	unsigned int lba,num_entries,clstr_no,free_clstr;
	char count,i,j,k,l=0,*fname,*ext,*test,*lname;
	bool found,start;
	lname = (char*) klm.kdmalloc (strlen(name)+1);
        strcpy(lname,name);
	dir_name=(unsigned char *)klm.kdmalloc(12);
	fname=(char *)klm.kdmalloc(9);
	ext=(char *)klm.kdmalloc(4);
	test=(char *)klm.kdmalloc(sizeof(strlen(lname)+1));
	clstr=(unsigned char *)klm.kdmalloc(sec_per_clstr*512);
	fat=(unsigned char *)klm.kdmalloc(512);
	
	if((file==CREATE)||(file==CREATE_DIR))
	{
		name_offset=0;
		*fname='\0';
		*ext='\0';
		for(i=0;*(lname+i)!='\0';)
		 {
			if(*(lname+i)=='/')
			{
				name_offset++;
				i++;
				temp=(unsigned char *)lname+i;
				count=0;
				while((*(lname+i)!='/')&&(*(lname+i)!='\0'))
				{
					count++;
					i++;
				}
				if(*(lname+i)!='\0')
				{
					for(j=0;j<count;j++)
					name_offset++;
				}
				else
				{
					name_offset--;
					if(file==CREATE)
					{
						for(j=0;j<strlen((char *)(temp));j++)
						{
							if(*(temp+j)=='.')
							{
								count=0;
								while((*(temp+j+1)!='\0')&&(count<3))
								{
									j++;
									if(*(temp+j)==' ')
									continue;
									*(ext+count)=*(temp+j);
									count++;
								}
								*(ext+count)='\0';
								if(*(temp+j+1)=='\0')
								*(temp+j-count)='\0'; 
         						}
       						}
					}
					for(j=0;(j<strlen((char *)(temp)))&&(j<8);j++)
					{
						if(*(temp+j)==' ')
						continue;
						*(fname+j)=*(temp+j);
					}
					*(fname+j)='\0';
					if(strlen((char *)(temp))>8)
					{
						count=0;
						found=1;
						while(found)
						{
							count++;
							*(fname+j-2)='~';
							*(fname+j-1)=count+48;
							if(name_offset)
							{
								*(lname+name_offset)='\0';
								start=1;
							}
							else
							{
								name_offset++;
								*(lname+name_offset)='\0';
								start=0;
							}
							strcpy(test,lname);
							if(start==1)
							*(test+name_offset)='/';
							for(k=start;k<(strlen(fname)+start);k++)
							*(test+name_offset+k)=*(fname+k-start);
							if(file==CREATE)
							{
								if(strlen(ext))
								{
									*(test+name_offset+i)='.';
									k++;
								}
								for(l=0;l<strlen(ext);l++)
								*(test+name_offset+k+l)=*(ext+l);
							}
							*(test+name_offset+k+l)='\0';
							found=fs_search(test,0);
						}
					}
					else
					{
						found=0;
						if(name_offset)
						{
							*(lname+name_offset)='\0';
							start=1;
						}
						else
						{
							name_offset++;
							*(lname+name_offset)='\0';
							start=0;
						}
						strcpy(test,lname);
						if(start==1)
						*(test+name_offset)='/';
						for(k=start;k<(strlen(fname)+start);k++)
						*(test+name_offset+k)=*(fname+k-start);
						if(file==CREATE)
						{
							if(strlen(ext))
							{
								*(test+name_offset+i)='.';
								k++;
							}
							for(l=0;l<strlen(ext);l++)
							*(test+name_offset+k+l)=*(ext+l);
						}
						*(test+name_offset+k+l)='\0';
						//disp1.printf("%s",test);
						found=fs_search(test,0);
						if(found)
						{
							disp1.printf("\nFILE WITH THE NAME ALREADY EXIST\n");
							return 0;
						}
					}
				}
			}
			else
			{
				disp1.printf("\nERROR IN PATH TO FILE\nCREATION OF %s UNSUCCESFULL\n",lname);
				return(0);
			}
		}
		if(name_offset)
		*(lname+name_offset)='\0';
		else
		{
			name_offset++;
			*(lname+name_offset)='\0';
		}
	}

	temp=clstr;
	clstr_no=root_dir_clstr_no;
	name_offset=0;
	while(1)
	{
		name_offset++;
		history=name_offset;
		while((*(lname+name_offset)!='/')&&(*(lname+name_offset)!='\0'))
		{
			*(dir_name+name_offset-history)=*(lname+name_offset);
			name_offset++;
		}
		if(strcmp(lname,"/")==0)
		flag=0;
		else
		{
			do
			{
				lba=clstr_lba(clstr_no);
				clstr=temp;
				drv->pioread(lba,sec_per_clstr,clstr);
				num_entries=0;
				while(num_entries<sec_per_clstr*16)
				{
					flag=0;count=0;
					while((count<8)&&(*(clstr+count)!=0x20))
					{
						if(*(dir_name+count)!= *(clstr+count))
						{
							flag=1;
							break;
						}
						count++;
					}
					if(((*(clstr+count)==0x20)||(count==8))&&(count<(name_offset-history)))
					{
						if(*(dir_name+count)=='.')
						{
							if(*(dir_name+count+1))
							{
								if(*(dir_name+count+1)==*(clstr+8))
								{
									if(*(dir_name+count+2))
									{
										if(*(dir_name+count+2)==*(clstr+9))
										{
											if(*(dir_name+count+3))
											{
												if(*(dir_name+count+3)==*(clstr+10)) break;
											}
											else break;
										}
									}
									else break;
								}
							}
							else break;
						}
						flag=1;
					}
					if(!flag)
					break;
					clstr+=32;
					num_entries++;
				}
				if(!flag)
				break; 
			}while(clstr_no=fat_lookup(clstr_no));
		}
		if((!flag)&&(*(lname+name_offset)=='\0'))
		{
			if(file==OPEN)
			{
				ft_ptr->next=(file_table *)klm.kdmalloc(sizeof(file_table));
				ft_ptr=ft_ptr->next;
				ft_ptr->next=0;
				ft_ptr->fentry_lba=lba;
				ft_ptr->fentry_offset=num_entries*8;
				ft_ptr->file_start_clstr_no=0;
				for(count=1;count>=0;count--)
				{
					ft_ptr->file_start_clstr_no=ft_ptr->file_start_clstr_no<<8;
					ft_ptr->file_start_clstr_no|=*(clstr+20+count);	
				}
				for(count=1;count>=0;count--)
				{
					ft_ptr->file_start_clstr_no=ft_ptr->file_start_clstr_no<<8;
					ft_ptr->file_start_clstr_no|=*(clstr+26+count);	
				}
				for(count=3;count>=0;count--)
				{
					ft_ptr->file_size=ft_ptr->file_size<<8;
					ft_ptr->file_size|=*(clstr+28+count);	
				}
			}
			else if(file==DELETE)
			{
				clstr_no=0;
				for(count=1;count>=0;count--)
				{
					clstr_no=clstr_no<<8;
					clstr_no|=*(clstr+20+count);	
				}
				for(count=1;count>=0;count--)
				{
					clstr_no=clstr_no<<8;
					clstr_no|=*(clstr+26+count);	
				}

				*clstr=0xe5;
				clstr=temp;
				drv->piowrite(lba,sec_per_clstr,clstr);
				if(clstr_no>2)
				{
					while(1)
					{
						lba=fat_begin_lba;
						lba+=clstr_no/128;
						drv->pioread(lba,1,fat);
						num_entries=clstr_no%128;
						free_clstr=*((unsigned int *)fat+num_entries);
						free_clstr&=0x0FFFFFFF;
						if(free_clstr<0x0FFFFFF8)
						{
							*((unsigned int *)fat+num_entries)=0x0;
							drv->piowrite(lba,1,fat);
							lba=clstr_lba(free_clstr);
							clstr_no=free_clstr;
						}
						else break;
					}
					*((unsigned int *)fat+num_entries)=0x0;
					drv->piowrite(lba,1,fat);
				}
			}
			else if(file==CREATE)
			{
				if(strcmp(lname,"/")==0)
				clstr_no=2;
				else
				{
					clstr_no=0;
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+20+count);	
					}
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+26+count);	
					}
				}
				flag=0;
				while(1)
				{
					lba=clstr_lba(clstr_no);
					clstr=temp;
					drv->pioread(lba,sec_per_clstr,clstr);
					//find a free slot for file entry
					num_entries=0;
					while(num_entries<sec_per_clstr*512)
					{
						if((*(clstr+num_entries)==0xE5)||(*(clstr+num_entries)==0x0))
						{
							for(count=0;count<8;count++)
							{
								if(count<strlen(fname))
								*(clstr+num_entries+count)=*(fname+count);
								else if(count)
								*(clstr+num_entries+count)=0x20;
							}
							for(count=0;count<3;count++)
							{
								if(count<strlen(ext))
								*(clstr+num_entries+8+count)=*(ext+count);
								else
								*(clstr+num_entries+8+count)=0x20;
							}
							*(clstr+num_entries+11)=0x00;
							*((unsigned short int *)clstr+(num_entries/2)+10)=0;
							*((unsigned short int *)clstr+(num_entries/2)+13)=0;
							*((unsigned int *)clstr+(num_entries/4)+7)=0;
							drv->piowrite(lba,sec_per_clstr,clstr);
							flag=1;
							break;
						}
						num_entries+=32;
					}
					if(flag)break;
					clstr_no=fat_lookup(clstr_no);
				}
			}
			else if (file == LIST)
			{
				if(strcmp(lname,"/")==0)
				clstr_no=2;
				else
				{
					clstr_no=0;
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+20+count);	
					}
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+26+count);	
					}
				}
				while(1)
				{
					lba=clstr_lba(clstr_no);
					clstr=temp;
					//unsigned char *clstr1=(unsigned char *)klm.kdmalloc(sec_per_clstr*512);
					drv->pioread(lba,sec_per_clstr,clstr);
					num_entries=0;
					while(num_entries<sec_per_clstr*512)
					{
						if ((*(clstr+num_entries)==0xE5)||(*(clstr+num_entries)==0x41)||((*(clstr+num_entries+11)==0x0F)))
						{
							num_entries+=32;
							continue;
						}
						else if (*(clstr+num_entries)==0x0)
						{
							flag=1;
							break;
						}
						else
						{
							if(*(clstr+num_entries+11)==0x10) disp1.setattrib(0x01);
							else disp1.setattrib(0x02);
							for(count=0;count<8&&(*(clstr+num_entries+count)!=0x20);count++)
							disp1.printf("%c",*(clstr+num_entries+count));
							if(*(clstr+num_entries+8)!=0x20)
							{
								disp1.printf(".%c",*(clstr+num_entries+8));
								for(count=1;count<3&&(*(clstr+num_entries+8+count)!=0x20);count++)
								disp1.printf("%c",*(clstr+num_entries+8+count));
							}
							disp1.printf(" ");
							disp1.setattrib(0x0f);
							num_entries+=32;
						}
					}
					if(flag)break;
					clstr_no=fat_lookup(clstr_no);
				}
				disp1.printf("\b\n");
			}
			else if(file==CREATE_DIR)
			{
				free_clstr=clstr_no;
				if(strcmp(lname,"/")==0)
				clstr_no=2;
				else
				{
					clstr_no=0;
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+20+count);	
					}
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+26+count);	
					}
				}
				flag=0;
				while(1)
				{
					lba=clstr_lba(clstr_no);
					clstr=temp;
					drv->pioread(lba,sec_per_clstr,clstr);
					//find a free slot for file entry
					num_entries=0;
					while(num_entries<sec_per_clstr*512)
					{
						if((*(clstr+num_entries)==0xE5)||(*(clstr+num_entries)==0x0))
						{
							for(count=0;count<8;count++)
							{
								if(count<strlen(fname))
								*(clstr+num_entries+count)=*(fname+count);
								else
								*(clstr+num_entries+count)=0x20;
							}
							*(clstr+num_entries+count)=0x20;
							*(clstr+num_entries+count+1)=0x20;
							*(clstr+num_entries+count+2)=0x20;
							*(clstr+num_entries+11)=0x10;
							if(dot==0)
							{
								free_clstr=new_clstr(0,fat);
disp1.printf("%d",free_clstr);
								*((unsigned short int *)clstr+(num_entries/2)+13)=(unsigned short int)free_clstr&0xFFFF;
								free_clstr=(free_clstr&0xFFFF0000)>16;
								*((unsigned short int *)clstr+(num_entries/2)+10)=(unsigned short int)free_clstr&0xFFFF;
							}
							else if(dot==1)
							{
								free_clstr=clstr_no;
								*((unsigned short int *)clstr+(num_entries/2)+13)=free_clstr&0xFFFF;
								free_clstr=(free_clstr&0xFFFF0000)>16;
								*((unsigned short int *)clstr+(num_entries/2)+10)=free_clstr&0xFFFF;
							}
							else if(dot==2)
							{
								*((unsigned short int *)clstr+(num_entries/2)+13)=free_clstr&0xFFFF;
								free_clstr=(free_clstr&0xFFFF0000)>16;
								*((unsigned short int *)clstr+(num_entries/2)+10)=free_clstr&0xFFFF;
							}
							*((unsigned int *)clstr+(num_entries/4)+7)=0;
							drv->piowrite(lba,sec_per_clstr,clstr);
							flag=1;
							break;
						}
						num_entries+=32;
					}
					if(flag)break;
					clstr_no=fat_lookup(clstr_no);
				}
			}
			else if(file==CD)
			{
				if(strcmp(lname,"/")==0)
				clstr_no=2;
				else
				{
					clstr_no=0;
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+20+count);	
					}
					for(count=1;count>=0;count--)
					{
						clstr_no=clstr_no<<8;
						clstr_no|=*(clstr+26+count);	
					}
				}
				cd_lba=clstr_lba(clstr_no);
			}

			return(1);
		}
		else if(flag)
		return(0);
		else if((!flag)&&(*(lname+name_offset)!='\0'))
		{
			clstr_no=0;
			for(count=1;count>=0;count--)
			{
				clstr_no=clstr_no<<8;
				clstr_no|=*(clstr+20+count);
			}
			for(count=1;count>=0;count--)
			{
				clstr_no=clstr_no<<8;
				clstr_no|=*(clstr+26+count);
			}
		}
	}
}


fat32::fat32(unsigned int lba_addr,drive *get_drv)
{
	unsigned char *mbr,*vbr;
	unsigned int lba=0;
	char count;
	drv=get_drv;
	lba=lba_addr;
	fs_begin_lba=lba;
	vbr=(unsigned char *)klm.kdmalloc(512);
	drv->pioread(lba,1,vbr);
	sec_per_clstr=*(vbr+13);
	no_of_res_sec=0;
	for(count=1;count>=0;count--)
	{
		no_of_res_sec=no_of_res_sec<<8;
		no_of_res_sec|=*(vbr+14+count);
	}
	no_of_fat=*(vbr+16);
	sec_per_fat=0;
	for(count=3;count>=0;count--)
	{
		sec_per_fat=sec_per_fat<<8;
		sec_per_fat|=*(vbr+36+count);
	}
	root_dir_clstr_no=0;
	for(count=3;count>=0;count--)
	{
		root_dir_clstr_no=root_dir_clstr_no<<8;
		root_dir_clstr_no|=*(vbr+44+count);
	}
	fat_begin_lba=fs_begin_lba+no_of_res_sec;
	clstr_begin_lba=fat_begin_lba+(no_of_fat*sec_per_fat);
}


unsigned int fat32::clstr_lba(unsigned int clstr_no)
{
	unsigned int lba;
	lba=clstr_begin_lba+(clstr_no-2)*sec_per_clstr;
	return lba; 
}


unsigned int fat32::new_clstr(unsigned int curr_clstr,unsigned char *fat)
{
	unsigned int lba,free_clstr_no,free_clstr,flag3,offset; 
	lba=fat_begin_lba;   
	free_clstr_no=2;
	flag3=0;

	while((!flag3)&&((lba-fat_begin_lba)<sec_per_fat))
	{
		drv->pioread(lba,1,fat);
		offset=2;
		while(offset<128)
		{
			free_clstr=*((unsigned int *)fat+offset);
			if(free_clstr==0)
			{
				flag3=1;
				break;
			}
			offset+=1;
			free_clstr_no+=1;
		}
		lba+=1;
	}
	if(!flag3)
	{
		disp1.printf("\nNO SPACE IN DISK\n");
		return 0;
	}
	else
	{
		if(curr_clstr>1)
		{
			lba=fat_begin_lba;
			lba+=curr_clstr/128;
			drv->pioread(lba,1,fat);
			offset=curr_clstr%128;
			*((unsigned int *)fat+offset)=free_clstr_no;
			drv->piowrite(lba,1,fat); 
		}
      
		curr_clstr=free_clstr_no;

		lba=fat_begin_lba;
		lba+=curr_clstr/128;
		drv->pioread(lba,1,fat);
		offset=curr_clstr%128;
		*((unsigned int *)fat+offset)=0x0FFFFFFF;
		drv->piowrite(lba,1,fat);

		lba=clstr_lba(curr_clstr);
		for(offset=0;offset<512;offset++) *(fat+offset)=0x0;
		for(offset=0;offset<sec_per_clstr;offset++)
		{
			drv->piowrite(lba+offset,1,fat);
		}

		return curr_clstr;
	}
}


bool fat32::fcreate(char *name)
{
	bool found=0;
	found=fs_search(name,CREATE);
	if(found)
	{
		//disp1.printf("\nCREATION OF %s SUCCESFULL\n",name);
		return 1;
	}
	else
	{
		disp1.printf("\nCREATION OF %s UNSUCCESFULL\n",name);
		return 0;
	}
}


int fat32::fopen(char *name,char* mode)
{
	bool found;
	found=fs_search(name,OPEN);
	if(!found)
	{
		disp1.printf("\nFILE NOT FOUND\n");
		return -1;
	}
	else
	{
		ft_ptr->fptr=0;
		ft_ptr->partition=this;
		if(!strcmp(mode,"R"))
		{
			rid+=2;
			if(rid%3==0)
			rid+=2;
			ft_ptr->fid=rid;
			//disp1.printf("\nFILE OPENED IN READ MODE\n");
			return(rid);
		}
		else if(!strcmp(mode,"W"))
		{
			wid+=3;
			if(wid%2==0)
			wid+=3;
			ft_ptr->fid=wid;
			//disp1.printf("\nFILE OPENED IN WRITE MODE\n");
			return(wid);
		}
		else if(!strcmp(mode,"RW"))
		{
			rwid+=6;
			ft_ptr->fid=rwid;
			//disp1.printf("\nFILE OPENED IN READ-WRITE MODE\n");
			return(rwid);
		}
		else
		{
			disp1.printf("\nINVALID MODE\n");
			return -1;
		}
	}
}


unsigned int fat32::fread(file_table *traverse,unsigned char **temp,unsigned int count)
{
	unsigned int tmp,sec_count=0,no_of_clstrs=0;
	unsigned int lba=0,clstr_no=0;
	unsigned char i,*buffer;

	if(((traverse->fptr)+count)>(traverse->file_size))
	count=(traverse->file_size)-(traverse->fptr);
	if(count)
	{
		sec_count=((traverse->fptr)/512)%sec_per_clstr;
		if((traverse->fptr%512)!=0)
		sec_count+=1;
		if(sec_count/sec_per_clstr)
		{
			no_of_clstrs+=1;
			sec_count=0;
		}
		tmp=512-(traverse->fptr%512);
		if(count>tmp)
		{
			tmp=count-tmp;
			sec_count+=tmp/512;
			if(sec_count/sec_per_clstr)
			{
				no_of_clstrs+=sec_count/sec_per_clstr;
				sec_count=sec_count%sec_per_clstr;
			}
		}
		no_of_clstrs+=1;
		buffer=(unsigned char *)klm.kdmalloc(no_of_clstrs*sec_per_clstr*512);
		clstr_no=traverse->file_start_clstr_no;
		lba=clstr_lba(clstr_no);
		lba+=int(traverse->fptr/(sec_per_clstr*512))*sec_per_clstr;
		for(i=0;i<no_of_clstrs;i++)
		{
			drv->pioread(lba,sec_per_clstr,buffer+(i*512*sec_per_clstr));
			clstr_no=fat_lookup(clstr_no);
			if(!clstr_no) break;
			lba=clstr_lba(clstr_no);
		}
		*temp=buffer+(traverse->fptr%(sec_per_clstr*512));
		traverse->fptr+=count;
	}
	else
	disp1.printf("\nCANNOT READ: COUNT=0 OR FILE POINTER=EOF\n");
	//disp1.printf("\n%d BYTES READ\n",count);
	return(count);
}


unsigned int fat32::fwrite(file_table *traverse,unsigned char *buffer,unsigned int count)
{
	unsigned int no_of_clstrs,tmp,flag1=0,flag2=0,offset;
	unsigned int i,lba=0,free_clstr_no,free_clstr,curr_clstr;
	unsigned char *temp,*fat;
	unsigned short int clstr_part;
	temp=(unsigned char *)klm.kdmalloc(sec_per_clstr*512);
	fat=(unsigned char *)klm.kdmalloc(512);
	no_of_clstrs=traverse->fptr/(sec_per_clstr*512);
	curr_clstr=traverse->file_start_clstr_no;
	if(!curr_clstr)
	{
		curr_clstr=new_clstr(curr_clstr,fat);
		if(!curr_clstr)
		{
			count=0;
			return(count);
		}
		else
		{
			lba=ft_ptr->fentry_lba;
			drv->pioread(lba,1,fat);
			clstr_part=(unsigned short int)(((curr_clstr&0xFFFF0000)>>16)&0x0000FFFF);
			offset=(ft_ptr->fentry_offset)*2+10;
			*((unsigned short int *)fat+(unsigned short int)offset)=clstr_part;
			clstr_part=(unsigned short int)(curr_clstr&0x0000FFFF);
			offset=(ft_ptr->fentry_offset)*2+13;
			*((unsigned short int *)fat+(unsigned short int)offset)=clstr_part;
			drv->piowrite(lba,1,fat);
			traverse->file_start_clstr_no=curr_clstr;
		}
	}	

	for(i=0;i<no_of_clstrs;i++)
	{
		lba=fat_begin_lba;
		lba+=curr_clstr/128;
		drv->pioread(lba,1,fat);
		offset=curr_clstr%128;
		curr_clstr=*((unsigned int *)fat+offset);
	}

	lba=clstr_lba(curr_clstr);
	drv->pioread(lba,sec_per_clstr,temp);
	if(count<=((sec_per_clstr*512)-(traverse->fptr%(sec_per_clstr*512))))
	tmp=(traverse->fptr+count)%(sec_per_clstr*512);
	else 
	{
		tmp=sec_per_clstr*512;
		flag1=1;
	}
	for(i=(traverse->fptr)%(sec_per_clstr*512);i<tmp;i++)
	*(temp+i)=*(buffer+i-((traverse->fptr)%(sec_per_clstr*512)));
	buffer+=(tmp-((traverse->fptr)%(sec_per_clstr*512)));
	if(!(traverse->fptr%(sec_per_clstr*512)))
	flag2=1;

	no_of_clstrs=0;
	if(count&&(traverse->fptr%(sec_per_clstr*512)))
	no_of_clstrs+=1;
	if(flag1)
	no_of_clstrs+=(count-(tmp-(traverse->fptr%(512*sec_per_clstr))))/(512*sec_per_clstr)+1;
	if(flag2)
	no_of_clstrs=count/(sec_per_clstr*512);
	if((count-(count/(sec_per_clstr*512))*512))
	no_of_clstrs+=1;

	for(i=0;i<no_of_clstrs;i++)
	{
		if((i+1)==no_of_clstrs)
		{
			tmp=0;
			while(*(temp+tmp)!='\0') tmp++;
			for(;tmp<(512*sec_per_clstr);tmp++) *(temp+tmp)=0x0;
		}
		drv->piowrite(lba,sec_per_clstr,temp);
		if((i+1)<no_of_clstrs)
		{
			strcpy((char *)temp,(char *)buffer);
			buffer+=sec_per_clstr*512;

			/*code to find the lba of a free cluster and associate it with the file*/
			lba=fat_begin_lba;
			lba+=curr_clstr/128;
			drv->pioread(lba,1,fat);
			offset=curr_clstr%128;
			free_clstr=*((unsigned int *)fat+offset);
			free_clstr&=0x0FFFFFFF;
			if(free_clstr<0x0FFFFFF8)
			{
				lba=clstr_lba(free_clstr);
				curr_clstr=free_clstr;
			}
			else
			{
				curr_clstr=new_clstr(curr_clstr,fat);
				if(!curr_clstr)
				{
					count-=(no_of_clstrs-i)*sec_per_clstr*512;
					break;
				}
			}
			lba=clstr_lba(curr_clstr);
		}    
	}
	traverse->fptr+=count;
	if(traverse->file_size<traverse->fptr)
	traverse->file_size=traverse->fptr;

	/*code to update file attributes viz size, time of modification etc*/
	lba=ft_ptr->fentry_lba;
	drv->pioread(lba,1,fat);
	offset=ft_ptr->fentry_offset+7;
	*((unsigned int *)fat+offset)=traverse->file_size;
	drv->piowrite(lba,1,fat);
	
	//disp1.printf("\n%d BYTES WRITTEN\n",count);
	return(count);
}


int fat32::fseek(file_table *traverse,int offset,int from)
{
	int flag;
	switch(from)
	{
		case 0:if(offset>=0)
			{
				if(((traverse->fptr)+offset)>(traverse->file_size))
				traverse->fptr=traverse->file_size;
				else
				traverse->fptr=offset;
				return 	traverse->fptr;
			 }
			else
			return -1;
		break;	
		case 1:	if(((traverse->fptr)+offset)>(traverse->file_size))
			traverse->fptr=traverse->file_size;
			else if(((traverse->fptr)+offset)<0)
			traverse->fptr=0;			
			else
			traverse->fptr+=offset;
			return traverse->fptr;
		break;
		case 2:if(offset<=0)
			{
				if(((traverse->file_size)+offset)<0)
				traverse->fptr=0;
				else			
		 		traverse->fptr=traverse->file_size+offset;
				return traverse->fptr;
			 }				
			else
			return -1;
	}
}	


bool fat32::fclose(file_table *traverse)
{
	int flag=0;
	file_table *tmp=ft_head;
	
	while(tmp->next)
	{
		if(traverse==(tmp->next))
		{
			flag=1;
			break;
		}
		tmp=tmp->next;
	}
	if(flag==1)
	{
		tmp->next=traverse->next;
		klm.kdfree(&traverse,sizeof(file_table));
		//disp1.printf("\nFILE CLOSE SUCCESFULL\n");
		return 1;
	}
	else
	{
		disp1.printf("\nFILE CLOSE UNSUCCESFULL\n");
		return 0;
	}
}


bool fat32::fdelete(char *name)
{
	bool found=0;
	found=fs_search(name,DELETE);
	if(found)
	{
		//disp1.printf("\nDELETION OF %s SUCCESFULL\n",name);
		return 1;
	}
	else
	{
		disp1.printf("\nDELETION OF %s UNSUCCESFULL\n",name);
		return 0;
	}
}

bool fat32::flist(char *name)
{
	bool found=0;
	found=fs_search(name,4);
	if(found) return 1;
	else return 0;
}

bool fat32::fcreate_dir(char *name)
{
	bool found=0;
	char *temp;unsigned char len;
	len=strlen(name);
	temp=(char *)klm.kdmalloc(len+4);
	found=fs_search(name,5);
	strcpy(temp,name);
	temp[len]='/';
	temp[len+1]='.';
	temp[len+2]='\0';
	dot=1;
	found=fs_search(temp,5);
	temp[len+2]='.';
	temp[len+3]='\0';
	dot=2;
	found=fs_search(temp,5);
	dot=0;
	if(!found) return 0;
	else return 1;
}

bool fat32::fcd(char *name)
{
	bool found=0;
	found=fs_search(name,6);
	if(!found) return 0;
	else return 1;
}
