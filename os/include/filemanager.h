/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/

#ifndef FILEMANAGER_DOT_H

#define FILEMANAGER_DOT_H

#include <filesystem.h>
#include <memory.h>
#include <video.h>
#include <storage.h>
#include <common.h>

#define PRIMARY_MASTER		0
#define PRIMARY_SLAVE		1
#define SECONDARY_MASTER	2
#define SECONDARY_SLAVE		3

#define PRIMARY_IDE_DEVICE 1
#define SECONDARY_IDE_DEVICE 2
#define CDROM 3

#define FAT32 1
#define FAT16 2
#define FAT12 3
#define ISO9660 4

#define E_NOTFOUND 100

#define E_NO_DRIVE_LETTER 200

struct partition_list
{
	fs *partition;
	char drive_letter;
	int ptype;
	partition_list *next;
	partition_list *prev;
};

struct disk_info 
{
	char name[200];
	int sectors;
	struct disk_info *next;
};

struct recently_used_partitions
{
	partition_list *ppointer;
	int count;
	bool isvalid;
};

struct recently_used_file
{
	unsigned int fid;
	file_table *fpointer;
	int count;
	bool isvalid;
};

struct device_list
{
	drive *device;
	device_list *next;
	device_list *prev;
	int dtype;
	int dno;
}; 

class filemanager
{
	partition_list *phead,*ptail;
	device_list *dhead,*dtail;
	int number_of_partitions;
	int number_of_devices;
	int maxdno;
	unsigned int partition_status;
	recently_used_partitions rup1,rup2;
	recently_used_file ruf1,ruf2;
	
public:
	filemanager (){}
		
	void initialize ();
  
	int attach_device (drive*,int);
	int detach_device (int);
 
	int attach_partition (fs*,int);
	int detach_partition (char);

	int open (char *,char *);
	bool close (unsigned int);
	unsigned int read (unsigned int,unsigned char**,unsigned int);
	unsigned int write (unsigned int,unsigned char*,unsigned int);
	int seek(unsigned int,int,int);
	bool file_search (char *,unsigned char);
	bool create_file (char *);
	bool delete_file (char *);
	bool list (char *);
	bool cd (char *);
	bool create_dir (char *);
	void delete_directory (char *);
	void set_attribute (char *,unsigned short int);
	unsigned short int get_attribute (char*);
	void truncate (char *);
};  

#endif

