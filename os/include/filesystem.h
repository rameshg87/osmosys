#ifndef FILESYSTEM_DOT_H
#include<storage.h>
#define FILESYSTEM_DOT_H

#define	CREATE		1
#define	DELETE		2
#define	OPEN		3
#define	LIST		4
#define	CREATE_DIR	5
#define CD		6

class fs;

struct file_table
{
	unsigned int fid;
	unsigned int fentry_lba;
	unsigned int fentry_offset;
	unsigned int file_start_clstr_no;
	unsigned int file_size;
	unsigned int fptr;
	fs *partition;
	unsigned int pid;
	struct file_table *next;
};


class fs
{
public:
	fs (){}
	virtual bool fcreate(char *){}
	virtual int fopen(char *,char *){}
	virtual unsigned int fread(file_table *,unsigned char **,unsigned int){}
	virtual unsigned int fwrite(file_table *,unsigned char *,unsigned int){}
	virtual int fseek(file_table *,int,int){}
	virtual bool fs_search(char *,unsigned char){}
	virtual bool fclose(file_table *){}
	virtual bool fdelete(char *){}
	virtual bool flist(char *){}
	virtual bool fcd(char *){}
	virtual bool fcreate_dir(char *){}
};


class fat32 : public fs
{
	drive *drv;
	unsigned char sec_per_clstr;
	unsigned char no_of_fat;
	unsigned int sec_per_fat;
	unsigned short int no_of_res_sec;
	unsigned int fat_begin_lba;
	unsigned int clstr_begin_lba;
	unsigned int root_dir_clstr_no;

public:
	unsigned int fs_begin_lba;
	fat32(unsigned int,drive *);
	unsigned int new_clstr(unsigned int,unsigned char *);
	bool fcreate(char *);
	int fopen(char *,char *);
	unsigned int fread(file_table *,unsigned char **,unsigned int);
	unsigned int fwrite(file_table *,unsigned char *,unsigned int);
	int fseek(file_table *,int,int);
	bool fclose(file_table *);
	bool fdelete(char *);
	bool fs_search(char *,unsigned char);
	unsigned int fat_lookup(unsigned int);
	unsigned int clstr_lba(unsigned int);
	bool flist(char *name);	
	bool fcreate_dir(char *);
	bool fcd(char *name);
};

class iso9660 : public fs
{
  struct primary_volume_descptr
  {
    unsigned char vdt;		//volume descrptr type
    unsigned char si[5];		//standard identifier
    unsigned char sid[32];		//system identifier
    unsigned char vid[32];		//volume identifier
    int vss;		//volume space size
    unsigned short int vsts;		//volume set size
    unsigned short int vsn;		//volume seq number
    unsigned short int lbs;		//logical block size
    unsigned int pts;		//path table size
    unsigned int lptb;		//location of l type path table
    unsigned int mptb;		//location of m type path table
    unsigned char rdr[34];		//root directory record
    unsigned char apid[128];	//application identifier identifier
    unsigned char vdnt[17];		//date and time of volume creation
    unsigned char mdnt[17];		//modified date and time
    unsigned char ednt[17];		//effective date and time
  };
  struct root_directory
  {
    unsigned char len_dir;		//length of the directory record
    unsigned char ext_attr_len;	//extended directory length
    unsigned int loc_ext;	//location of Extent
    unsigned int data_len;	//data length
    unsigned char date_time[7];	//recording date and time
    unsigned char file_flag;	//File flags
    unsigned char file_unit_size;	//File unit size
    unsigned char int_gap_size;	//Interleave gap size
    unsigned short int  vol_seq_num;	//Volume seq number
    unsigned char len_file_id;	//length of the file identifier
    unsigned char file_id[30];	//File identifier
    
    
  };
  struct path_table
  {
    unsigned char len_di;
    unsigned char ext_attr_len;
    unsigned int loc_ext;
    unsigned short int par_dir_no;
    unsigned char dir_id[30];
    unsigned char pad_fld[30];
    
  };
 
 public:
 primary_volume_descptr pr;
 root_directory rd;
 path_table pt;
 drive *drv;
 iso9660(drive*);
 int fopen(char *,char *);
 void load_rdr(int ,unsigned char *);
 void load_ptb(int ,unsigned char *);
 int end_file(unsigned char *);
 int fseek(file_table *,int,int); ////////////changeeeee
 unsigned int fread(file_table *,unsigned char **,unsigned int);
 bool fclose(file_table *);
 bool flist(char*);	
 bool move_to_dir(char*);	
 bool fcd(char*);	
 bool fs_search (char *,unsigned char);
};

#endif

