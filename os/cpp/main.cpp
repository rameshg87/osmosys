/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS

********************************************************************************/

#include <gdt.h>
#include <idt.h>
#include <video.h>
#include <memory.h>
#include <filesystem.h>
#include <storage.h>
#include <common.h>
#include <process.h>
#include <filemanager.h>

/*******************************************************************************/


/* KERNEL GLOBAL VARIABLES */

/* kernel logical memory object */
logical_memory klm; 

/* pointer to currently active memory object */
logical_memory *current_active_memory; 

/*display object */
display disp1; 

/* one and only physical memory object */
physical_memory pm; 

/* pointer to linked list file table */
file_table *ft_head; 
file_table *ft_ptr;

/* top level filemanager object */
filemanager fm;
disk_info *dih,*dit;

/* process id generator variable */
int processid_gen;

/* process scheduler object */
scheduler slr;

/* one and only tss used for process switch */
tss_entry_t tss;

/* variable to indicate round robin scheduling has started */
unsigned char round_robin_start=0;

/* interrupt synchronisation variables */
int irq1_flag;	/* keyboard */	
int irq2_flag;  /* shell_process  */
int irq6_flag;	/* floppy */
int irq14_flag;	/* primary disk controller */
int irq15_flag; /* secondary disk controller */

int irq1_status; /* keyboard status recogniser */

/* global file id generators */
unsigned short int rid,wid,rwid;

/* currently active process's pid */
int current_pid;

/* global descriptor table objects */
gdt_ptr gptr;
gdt_entry segment[1024];

/* interrupt descriptor table objects */
struct idt_entry idt_entries [256];
struct idt_ptr iptr;

/* system clock */
unsigned long sys_clock=0;

/* clock for scheduling */
unsigned int process_clock=0;

/* store temporary esp in time of context switch */
unsigned int temp;

/* keyboard scancode recorded */
char scancode;

/* who called last system call backup */
int flagger;

unsigned int cd_lba;

char *cd_name;

scancode_list *schead,*sctail;

/*******************************************************************************/


extern "C" void enable_paging ();

extern "C" void kmain ()
{  
	irq1_flag = -1;
	irq14_flag = -1;
	irq15_flag = -1;
	flagger = 0;
	dih=0;dit=0;
	round_robin_start = 0;
	slr.running_list_element = 0;
	unsigned short int fid;
	unsigned char **buffer;
	int count;


	/* initialize the text mode display */
	disp1.initialize (80,25,0xb8000);


	/* setup the gdt */
	gdt_setup ();



	/* setup the idt */
	idt_install ();

//	matrix ();

	disp1.setattrib (0x0f);
	disp1.clear_screen ();

	disp1.printf ("KERNEL LOADED");

	disp1.printf ("\n\nInitializing Display");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);


	disp1.printf ("\nSetting up GDT");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);

	disp1.printf ("\nSetting up IDT");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);


	disp1.printf ("\n\nInitializing Memory Manager");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);

	disp1.printf ("\nEnabling Paging");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);

	/* initialize the physical memory */
	pm.initialize ();

	/* klm - kernel logical memory */
	klm.initialize_memory(KERNEL_LOGICAL_MEMORY);

	/* identity map 0 - 8 MB mark */
	for (int i=0;i<0xB00000;i+=4096)
		klm.map (i,i,0x7);

	/* load the page directory of klm into cr3 */
	klm.load_page_directory ();



	/* enable paging */
	enable_paging ();


	current_active_memory = &klm;
  
	ft_head = (file_table *)klm.kdmalloc(sizeof(file_table));
	ft_head->next=0;
	ft_ptr = ft_head;

	disp1.printf ("\n\nInitializing File Manager");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);


	fm.initialize ();
	processid_gen = 0;
	//fm.cd("a:/OSMOSYS");

	slr.initialize ();

	disp1.printf ("\n\nStarting System Idle Process");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);

	disp1.printf ("\n\n");

	process *p1 = new process("a:/BIN/IDLEP");

	disp1.printf ("\nStarting Shell");
	disp1.gotoend ();
	disp1.setattrib (0x02);
	disp1.printf ("[Ok]");
	disp1.setattrib (0x0f);

	process *p2 = new process ("a:/BIN/SHELL");

 	slr.init_first_process ();
	
	for (;;);

}
