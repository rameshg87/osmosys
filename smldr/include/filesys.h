/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.1

********************************************************************************/

class fat_partition
{
  int partition_lba;

  int number_of_fat;
  int sectors_per_fat;
  int number_of_reserved_sectors;
  int cluster_begin_lba;
  int root_dir_lba;
  int fat_begin_lba;
public:
  int root_dir_cluster;
  fat_partition (int);
  int sectors_per_cluster;  
  int find_directory_entry (const char *filename,int db_index);
  int return_file_cluster (const char *filename,int dir_cluster_index,int dir_cluster,int *filesize);
  int find_chain_cluster (int cluster);
  void read_cluster (int cluster,int dbindex);
};


void read_sector (int,int);
int return_partition_lba (int);
