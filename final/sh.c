#include "ucode.c"

//example: cmdLine = "cat < infile | lower2upper | grep print > outfile"

//gets rid of spaces on the ends of commands in pipes
//this is since commands are ususally seperated from
//the pipe symbols by spaces
char *trimEnds(char *command)
{
	char *start = command, *end = command;

	//go to end of string
	while (*end != '\0')
	{
		end++;
	}
	end--;

	//keep removing spaces from the front
	while (*start == ' ')
	{
		//remove space, move forwards
		*start = '\0';
		start++;
	}

	//keep removing spaces from the end
	while (*end == ' ')
	{
		//remove space, move backwards
		*end = '\0';
		end--;
	}

	//return newly trimmed string
	return start;
}

//scan cmdLine for I/O redirection symbols and do redirections
//head = cmdLine BEFORE redirections
//redirects are handled by setting the stdin and stdout to
//the head and tail results of commands
int do_command(char* cmdLine)
{	
	int fd, redirect;
	char *command, *cp, symbol[3];
	char *head, *tail;

	symbol[0] = '\0';
	symbol[1] = '\0';
	symbol[2] = '\0';

	//copy cmdLine so we don't mess it up
	strcpy(command, cmdLine);

	//get rid of leading and trailing spaces
	command = trimEnds(command);

	//initialize head and tail
	head = command;
	tail = command + strlen(command) - 1;

	//set char pointer to the new command
	cp = command;

	//scan cmdLine for I/O redirection symbols
	while ((*cp != '>' && *cp != '<') && cp != tail)
	{
		cp++;
	}

	//check if redirect symbol found
	if ((*cp == '>' || *cp == '<') && cp != tail)
	{
		//found redirect!
		//save symbol (could be > or <)
		symbol[0] = *cp;

		//check if >
		if (*cp == '>')
		{
			//if it's > , we need to check if it's >>
			if (*(cp + 1) == '>')
			{
				symbol[1] = '>';//it's >>
			}
		}

		//null terminate
		*cp = '\0';

		//set head and tail again
		head = command;
		tail = cp + strlen(symbol);

		//set redirect flag
		redirect = 1;
	}
	else
	{
		//no redirect, set flag to 0
		redirect = 0;
	}

	//get rid of leading and trailing spaces
	head = trimEnds(head);

	//if redirect exists
	if (redirect)
	{
		//get rid of leading and tailing spaces
		tail = trimEnds(tail);
		
		//check redirect symbol
		if (symbol[0] == '>')
		{
			//redirect stdout to a file
			//check if append or just normal
			if (strlen(symbol) == 2)//append >>
			{
				//open (or create) tail for appending to
				fd = open(tail, O_WRONLY|O_CREAT|O_APPEND);
				dup2(fd, 1);// copy tail file descriptor to stdout
				close(fd);//close the original fd
			}
			else if (strlen(symbol) == 1)//normal >
			{
				//open tail for writing
				fd = open(tail, O_WRONLY|O_CREAT);
				dup2(fd, 1);//copy tail file descriptor to stdout
				close(fd);//close the original fd
			}
			else
			{
				//error
				printf("Error on Redirecting!");
				exit(4);
			}

			
		}
		else if (symbol[0] == '<')
		{
			//redirect input, open tail for reading to take in input
			fd = open(tail, O_RDONLY);
			dup2(fd, 0);//copy tail fd to stdin
			close(fd);
		}
		else
		{
			printf("ERROR: quiting process\n");
			exit(0);
		}
	}
	//run command
	exec(head);
}

//divide cmdLine into head and tail by rightmost | symbol
int scan(char *cmdLine, char **head, char **tail)
{
	char *cp = cmdLine;
	*tail = 0;

	//go to end of string
	while(*cp)
		cp++;

	//keep going back until either '|' or beginning
	while(cp != cmdLine && *cp != '|')
		cp--;

	//if reached beginning, no pipe
	if(cp == cmdLine)
		return 0;//no pipe

	//there's a pipe!

	//remove pipe symbol
	*cp = 0;
	cp++;

	//skip over blank spaces
	while(*cp == ' ')
		cp++;

	//set new head and tail
	*tail = cp;
	*head = cmdLine;

	return 1;//there's a pipe
}

int do_pipe(char *cmdLine, int *pd)
{
	char *head = cmdLine;
	char *tail = cmdLine + strlen(cmdLine) - 1;

	int hasPipe = 0;
	int lpd[2]; //pipe array [in, out]
	int pid;

	//check if pipe passed in, as WRITER on pipe pd
	if (pd)
	{
		//close stdout and make pd[1] the stdout
		close(pd[0]);//close read pipe
		dup2(pd[1], 1);//copy write pipe to stdout
		close(pd[1]);//close write pipe
	}

	//divide cmdLine into head, tail by rightmost pipe symbol
	hasPipe = scan(cmdLine, &head, &tail);

	if (hasPipe)//theres a pipe, we need to process it
	{
		//create a pipe lpd
		pipe(lpd);

		//fork a child to write
		pid = fork();

		if (pid)//parent
		{
			//as READER on lpd
			//Close the write end of the pipe
			close(lpd[1]);//close read pipe
			dup2(lpd[0], 0);//copy write to stdin
			close(lpd[0]);//close write pipe

			do_command(tail);//do the tail command
		}
		else//child
			do_pipe(head,lpd);//recursive call to function to handle multiple pipes

	}
	else//no pipe, just run command
		do_command(cmdLine);
}

//starting point of shell execution
main(int argc, char* argv[])
{
	char *cmdLine, *cp;
	char line[64], *commands[2];
	int status, exists = 0;

	printf("starting shell...\n");

	//shell keeps looping until process killed
	while (1)
	{
		//print the prompt.
		printf("ldsh # : ");

		//Get a command line from user
		gets(cmdLine);

		//trim it.
		cmdLine = trimEnds(cmdLine);

		//check for empty line.
		if (strcmp(cmdLine, "") == 0)
		{
			continue;
		}

		//logout (built in commands)
		if (strcmp(cmdLine, "exit") == 0 || strcmp(cmdLine, "logout") == 0)
		{
			printf("\n********LOGGING OUT********\n");
			exit(0);
		}

		//a little string manipulation to check for cd
		//copy line so strtok doesn't ruin our beautiful cmdLine
		strcpy(line, cmdLine);

		//get first two elements of the line
		commands[0] = strtok(line, " \t");
		commands[1] = strtok(0, "\r");
		//printf("c1=%s\n", commands[0]);
		//printf("c2=%s\n", commands[1]);

		//check if cd into directory
		if(strcmp(commands[0], "cd") == 0)
		{
			//if dir, cd into it. else cd into root
			if(commands[1])
			{
				printf("cd into %s\n", commands[1]);
				chdir(commands[1]);
			}
			else
			{
				printf("cd into root\n");
				chdir("/");
			}
			continue;
		}

		//for binary executable command.
		pid = fork();

		//check if parent or child
		if (pid)//parent sh
		{
			cp = cmdLine;

			//search cmdLine for &
			while ((*cp) != '\0')
			{
				if ((*cp) == '&')
				{
					exists = 1;
					break;
				}
				cp++;
			}

			//if no & symbol
			if (exists == 0)//assume at most one & for main sh
				pid = wait(&status);
			exists = 0;
			continue;//main sh does not wait if &
		}

		else//child sh
			do_pipe(cmdLine, 0);
	}
}
