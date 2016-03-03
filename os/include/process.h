
#ifndef PROCESS_DOT_H

#define PROCESS_DOT_H

#include <common.h>
#include <memory.h>
#include <filemanager.h>
#include<gdt.h>
#include<storage.h>

#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3

class process;

struct pm_list
{
	unsigned int address;
	struct pm_list *next; 	
};

struct process_list_element
{
	process *ptr;
	int test;
	process_list_element *next;
	process_list_element *prev;
};

class process_queue 
{

public:
	void initialize ();
	process_list_element *head,*tail;
	int count;
	process_queue () {head=0;tail=0;count=0;}
	void insert (process_list_element*);
	process_list_element* remove ();
	int getcount ();
};

class process_list
{
	process_list_element *begin,*end;  
	int count;

public:
	void initialize ();
	process_list (){count =0; begin =0; end=0;}
	void insert (process_list_element*);
	process_list_element* remove (unsigned int);
	int getcount ();
};

class scheduler
{
	process_list waiting;
	process_queue ready_queue[3];
public:
	void initialize ();
	process_list_element *running_list_element;
	void process_switch (int);
	void intiate_process(process*);
	void init_first_process ();
	void context_switch(process_list_element*);
	unsigned int return_current_active_pid ();
	void remove_from_waiting_list(unsigned int);
};

class process
{
	int process_state;
	unsigned short int priority;
	unsigned int pid;
	unsigned long int time_started;
	struct reg_val *ptr_to_correct_stack;

	char rootdir[500];
	char cwd[500];
	char executable[500];
	gdt_entry *ldt_entry;
	gdt_ptr *ldt_ptr;
	unsigned int esp;
        unsigned int es_selector;
	unsigned int cs_selector;
	unsigned int ss_selector;
	unsigned int ds_selector;
	unsigned int fs_selector;
	unsigned int gs_selector;
	unsigned int cr3;
	unsigned int eip;
	int codesegment;
	unsigned int ppid;
public:
	logical_memory *memobj;
	unsigned short int get_priority();
	process (char*);
	void change_state (int new_state);
	unsigned int get_pid ();
	unsigned int get_ppid ();
	unsigned int set_ppid (unsigned int);
	friend void scheduler::context_switch(process_list_element*);
	friend void scheduler::process_switch(int);
	friend void scheduler:: init_first_process ();
	void put_kernel_stack_value (struct reg_val*);
	unsigned int kernel_stack_value ();
	
};


#endif

