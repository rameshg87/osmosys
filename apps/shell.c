#include "syscalls.h"

unsigned char *strtok(unsigned char *string,unsigned char **rem_command)
{
	char i=0;
	while((*(string+i)!='\0')&&(*(string+i)!=' ')) i++;
	if (*(string+i)!='\0') *(string+i)='\0';
	*rem_command = string+i+1;
	return string;
}

void pmain ()
{
	unsigned char *string,pid,*command,*temp;
	unsigned char *name,*rem_command,*path_var;
	int found;
	print ("\n\n");
	path_var = "a:/BIN";
	string = cd("a:");

	while(1)
	{	
		found = 0;
		print("user@osmosys_shell ");
		print("%s# ",string);

		command=gets();
		print("\n");

		rem_command = (unsigned char *)malloc(strlen(command)+1);
		strcpy(rem_command,command);
		name = strtok(rem_command,&rem_command);
		if (strcmp(name,"cd")==0)
		{
			name = strtok(rem_command,&rem_command);
			string = cd(name);
		}
		else if (strcmp(name,"shutdown")==0)
		{
			shutdown();
		}
		else if (strcmp(name,"reboot")==0)
		{
			reboot();
		}
		else if (strcmp(name,"mkfile")==0)
		{
			name = strtok(rem_command,&rem_command);
			mkfile(name);
		}
		else if (strcmp(name,"mkdir")==0)
		{
			name = strtok(rem_command,&rem_command);
			mkdir(name);
		}
		else if (strcmp(name,"rm")==0)
		{
			name = strtok(rem_command,&rem_command);
			rm(name);
		}
		else if (strcmp(name,"cls")==0)
		{
			clrscr ();
			continue;
		}
		else if (strcmp(name,"sysinfo")==0)
		{
			sysinfo ();
			continue;
		}
		else
		{
			temp = (unsigned char *)malloc(strlen(command)+strlen(string)+2);
			strcpy (temp,string);
			temp[strlen(temp)+1] = '\0';
			temp[strlen(temp)] = '/';
			strcat (temp,command);

			found = file_search (temp,0);
		
			if(found)
			create_process(temp);

			else
			{
				temp = (unsigned char *)malloc(strlen(command)+strlen(path_var)+2);
				strcpy (temp,path_var);
				temp[strlen(temp)+1] = '\0';
				temp[strlen(temp)] = '/';
				strcat (temp,command);

				found = file_search (temp,0);
		
				if(found)
				create_process(temp);
				else
				{
					found = file_search(command,0);
					if(found)
					create_process(command);
					else
					print("NOT FOUND\n");
				}
			}
		}
	}
}

