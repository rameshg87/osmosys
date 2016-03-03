/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/


#ifndef FILESYSTEM_DOT_H
#define FILESYSTEM_DOT_H


#define MASTER 0
#define SLAVE 0x10

class drive
{

	
public:
	int driveno;
	drive();
	virtual void pioread(int,int,unsigned char* );
	virtual void piowrite(int,int,unsigned char* );
	virtual void dmaread(int,int,unsigned char* );
};

class primarydisk: public drive
{
unsigned char dtype;
public:
primarydisk(unsigned char,int);
void pioread(int,int,unsigned char* );
void piowrite(int,int,unsigned char* );
};

class secondarydisk: public drive
{
unsigned char dtype;
public:
secondarydisk (unsigned char,int);
void pioread(int,int,unsigned char*);
void piowrite(int,int,unsigned char*);
};

class floppy_drive:public drive
{
	public:
		floppy_drive();
		void dma_access(int,int,int);
};	

#endif
