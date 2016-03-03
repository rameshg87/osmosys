#include <process.h>
#include <video.h>
#include <gdt.h>

extern "C" void load_ldt (unsigned int);
extern "C" void restore_process_values();
extern "C" void srestore_process_values();
extern "C" void jmp_2_new_process();
extern logical_memory *current_active_memory;
extern unsigned char round_robin_start;
extern unsigned long sys_clock;
extern logical_memory klm;
extern int processid_gen;
extern scheduler slr;
extern filemanager fm;
extern display disp1;
extern int k;
extern gdt_entry segment[1024];
extern tss_entry_t tss;
extern int irq1_flag;
extern int irq2_flag;
extern int irq1_status;
extern int irq14_flag;
extern int irq15_flag;
extern int flagger;
extern int current_pid;
extern unsigned int temp;
extern char count;
extern unsigned long sys_clock;

process :: process (char *filename)
{
	/* open the executable file in read mode */
	unsigned short fid = fm.open (filename,"R");
	unsigned int readsize;
	unsigned char **tmpbuffer,*newbuffer;
	unsigned int virtual_address=0x2000000; 
	
	tmpbuffer=(unsigned char **)klm.kdmalloc(sizeof(unsigned char *));
	priority=2;
	pid=processid_gen++;         //bad programming idea....to be changed
	memobj = new logical_memory ();
	memobj->initialize_memory (USER_LOGICAL_MEMORY);
	
	fm.read (fid,tmpbuffer,4);
	codesegment = return_int (*tmpbuffer,0,1);

	readsize = fm.read (fid,tmpbuffer,4096);


        while (readsize == 4096)
	{			
		memobj->map (virtual_address,klm.get_physical_address ((unsigned int)*tmpbuffer),0x7);
		klm.set_page_attribute ((unsigned int)*tmpbuffer,0x800);
		readsize = fm.read (fid,tmpbuffer,4096);
		virtual_address+=4096;
	}

	
	if (readsize != 0)
	{
		newbuffer = (unsigned char*)klm.kdmalloc (4096);
		memcpy ((char*)*tmpbuffer,(char*)newbuffer,readsize);
		memobj->map (virtual_address,klm.get_physical_address ((unsigned int)newbuffer),0x7);  
	//	klm.kdfree (*tmpbuffer,readsize);
	}




	process_state=NEW;
	unsigned int esp_gen=(unsigned int)klm.kdmalloc(4096);



	esp=esp_gen+4096;
	slr.intiate_process(this);


	
	gs_selector = 0x2b;
	fs_selector = 0x2b;
	ds_selector = 0x2b;
	ss_selector = 0x2b;
	es_selector = 0x2b;
	cs_selector = 0x23;
	eip = 0;
	cr3 = (unsigned int) (memobj -> get_page_directory_address ());



}

void process :: put_kernel_stack_value (struct reg_val *ptr)
{
	ptr_to_correct_stack = ptr;
}

unsigned int process :: get_pid ()
{
	return pid;
}

unsigned int process :: get_ppid ()
{
	return ppid;
}

unsigned int process :: set_ppid (unsigned int value)
{
	ppid=value;
}

unsigned short int process::get_priority()
{
	return priority;
}

void process_queue :: initialize ()
{
	head = 0;
	tail = 0;
	count = 0;
}

void process_queue::insert(process_list_element* ple)
{


	if (head == 0)
	{	
		head = ple;
		tail = head;
		tail -> prev = 0;
		head -> next = 0;
	}
	else
	{
		tail -> next = ple;
		ple -> prev = tail;
		ple -> next = 0;
		tail = ple;
	}
	count++;


}


process_list_element* process_queue :: remove ()
{
	process_list_element *tmp = head;
	head = head -> next;
	head ->prev = 0;
	if (head == 0)tail = 0;
	count--;
	return tmp;
}


int process_queue :: getcount ()
{
	return count;
}

void process_list :: initialize ()
{
	begin = 0;
	end = 0;
	count = 0;
}


void process_list :: insert (process_list_element* ple)
{
	
	if (begin == 0)
	{
		begin = ple;
		end = begin ;
		end -> prev = 0;
		begin-> prev = 0;
		end -> next = 0;
		begin-> next = 0;
	}
	else
	{
		end -> next = ple;
		ple -> prev = end ;
		ple -> next = 0;
		end = ple;
	}	
	count ++;
}

process_list_element* process_list :: remove (unsigned int pid)
{
	process_list_element *tmp = begin;
	
	while (tmp != 0)
	{
		if (tmp -> ptr -> get_pid() == pid)
		{
			if (tmp -> next) tmp -> next -> prev = tmp -> prev;
			if (tmp -> prev) tmp -> prev -> next = tmp -> next;
			if (tmp == end ) { if(tmp->prev) end=tmp->prev; else end=0; }
			if (tmp == begin){ if(tmp->next) begin=tmp->next; else begin=0; } 
			count --;
			return tmp;
		}
		tmp = tmp -> next;
	}
	return 0;
}



int process_list :: getcount ()
{
	return count;
}


void scheduler :: process_switch (int pid)
{
	process_list_element *old_process=running_list_element;
	if (pid == -1)
	{
		//timer interrupt
		//to be improved
		ready_queue[running_list_element->ptr->get_priority()].insert (running_list_element);
			
		if (ready_queue [0].getcount () != 0)
			running_list_element = ready_queue [0].remove ();
		else if (ready_queue [1].getcount () != 0)
			running_list_element = ready_queue [1].remove ();
		else if (ready_queue [2].getcount () != 0)
		{
			running_list_element = ready_queue [2].remove ();

			if (running_list_element -> ptr -> get_pid () == 0 && ready_queue[2].getcount() >= 1)
			{
				ready_queue[2].insert (running_list_element);
				running_list_element = ready_queue [2].remove ();
			}
		}
		else 
		{
			return;
		}
		old_process->ptr->process_state=READY;
	
		context_switch(old_process);    
	}
	else
	{
		//transfer a process from waiting queue to  ready queue
		if(pid==current_pid)
		{
			waiting.insert(running_list_element);
			if (ready_queue [0].getcount () != 0)
			running_list_element = ready_queue [0].remove ();
			else if (ready_queue [1].getcount () != 0)
			running_list_element = ready_queue [1].remove ();
			else
			{
				running_list_element = ready_queue [2].remove ();
				if (running_list_element -> ptr -> get_pid () == 0 && ready_queue[2].getcount() >= 1)
				{
					ready_queue[2].insert (running_list_element);
					running_list_element = ready_queue [2].remove ();
				}
			}
		old_process->ptr->process_state=WAITING;
		context_switch(old_process);
		}
	      else 
		{
			process_list_element *temp=waiting.remove(pid);
			temp->ptr->process_state=READY;
			ready_queue[temp->ptr->get_priority()].insert(temp);
		}
	}
}

void scheduler::remove_from_waiting_list(unsigned int value)
{
	waiting.remove(value);
}

void scheduler::intiate_process(process* pptr)
{


	process_list_element* new_process;
	new_process=(process_list_element*)klm.kdmalloc(sizeof(process_list_element));
	new_process -> ptr = pptr;

	ready_queue[pptr->get_priority()].insert(new_process);




}

void scheduler::context_switch(process_list_element *old_process)
{
	int i;
	unsigned int ret_val;
	
        //new segment entries are defined for the new process
	segment [3] = gdt_assign (0xfffff,0x0,0xfa,0xc);               

	//dummy user code segment
        segment [4] = gdt_assign (0xfffff,0x0,0xf2,0xc);   //dummy user data segment
	segment [6] = gdt_assign (0xfffff,0x0,0xf2,0xc);    //stack segment for user processes.

	current_active_memory = running_list_element -> ptr -> memobj;
	current_pid = running_list_element -> ptr -> get_pid ();
	
	 tss.esp0=running_list_element->ptr->esp;
	running_list_element->ptr->memobj->load_page_directory();
	if(running_list_element->ptr->process_state==READY)
	{
		temp = running_list_element->ptr->kernel_stack_value();
		restore_process_values();
	}
	else
	{	
                running_list_element->ptr->process_state=RUNNING;
		jmp_2_new_process();	
	}
}

unsigned int scheduler :: return_current_active_pid ()
{
	return running_list_element -> ptr -> get_pid ();
}

void scheduler :: init_first_process ()
{
	running_list_element = ready_queue [2].remove ();
	running_list_element->ptr->process_state=RUNNING;
        //new segment entries are defined for the new process
	segment [3] = gdt_assign (0xfffff,0x0,0xfa,0xc);               

	//dummy user code segment
        segment [4] = gdt_assign (0xfffff,0x0,0xf2,0xc);   //dummy user data segment
	segment [6] = gdt_assign (0xfffff,0x0,0xf2,0xc);    //stack segment for user processes.
	
        tss.esp=running_list_element->ptr->esp;
	running_list_element->ptr->memobj->load_page_directory();
	round_robin_start=1;
	jmp_2_new_process();	
}

void scheduler :: initialize ()
{
	int i;
	for (i=0;i<3;i++)
		ready_queue [i]. initialize ();
	waiting. initialize ();
}

unsigned int process :: kernel_stack_value ()
{
	return (unsigned int)ptr_to_correct_stack;
}

extern "C" void newswitch (struct reg_val *reg)
{

	slr.running_list_element -> ptr -> put_kernel_stack_value (reg);
	unsigned int *num = (unsigned int*)slr.running_list_element->ptr->kernel_stack_value();

	switch (flagger)
	{
	case 1:
		slr.process_switch (irq1_flag);
		break;
	case 2:
		slr.process_switch (irq2_flag);
		break;
	case 14:
		slr.process_switch (irq14_flag);
		break;
	case 15:
		slr.process_switch (irq15_flag);break;
	}
}
