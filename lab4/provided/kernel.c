/*************************************
Luke Darrow 11349190
CptS 460 Lab 4 kernel.c
*************************************/

//as shown in KC's textbook
PROC *kfork() // create a child process, begin from body()
{
	int i;

	//get the proc from the free list
	PROC *p = get_proc(&freeList);

	//make sure you got a process!
	if (!p)
	{
		printf("no more PROC, kfork() failed\n");
		return 0;
	}

	//set the status and priority and parent id
	p->status = READY;
	p->priority = 1;					// priority = 1 for all proc except P0
	p->ppid = running->pid;				// parent = running

	/* initialize new proc's kstack[ ] */ 
	for (i=1; i<10; i++)				// saved CPU registers
		p->kstack[SSIZE-i]= 0 ;			// all 0's to show that it's an "empty process"

	p->kstack[SSIZE-1] = (int)body;		// resume point=address of body()
	p->ksp = &p->kstack[SSIZE-9];		// proc saved sp
	enqueue(&readyQueue, p);			// enter p into readyQueue by priority

	return p;							// return child PROC pointer
}

//just calls switch
int do_tswitch()
{
	printf("proc %d tswitch()\n", running->pid);
	tswitch();
	printf("proc %d resumes\n", running->pid);
}

//as shown in KC's textbook
int do_kfork()
{
	//get pointer to child process from kfork()
	PROC *p = kfork("/u1");

	//if 0, kfork failed!
	if (p == 0)
	{
		printf("kfork failed\n");
		return -1;
	}
	else
	{
		//printf("child pid = %d\n", p->pid);

		//print results
		printf("PROC %d kfork a child %d\n", running->pid, p->pid);
		//return the pid of the newly forked process
		return p->pid;
	}
}

//changes running process status to ZOMBIE, then calls tswitch() to give up CPU
int do_exit()
{
	int exitValue;

	if (running->pid == 1 && nproc > 2){
		printf("other procs still exist, P1 can't die yet\n");
		return -1;
	}

	//ask for exitValue (value), e.g. 123
	printf("enter an exitValue (0-9): ");
	//rgets(exitValue);
	exitValue = (getc()&0x7F) - '0';
	//call kexit converting exitValue to integer
	kexit(exitValue);
}

//stops process to wait for an event
int do_sleep()
{
	int event = 0;
	//ask for eventValue (value), e.g. 123
	printf("enter an eventValue: ");
	//rgets(input);
	event = (getc()&0x7F) - '0';

	//call kexit after converting input to integer
	ksleep(event);
}

//wakeup sleeping process for event
int do_wakeup()
{
	int event = 0;

	//ask for eventValue (value), e.g. 123
	printf("enter an exitValue: ");
	//rgets(input);
	event = (getc()&0x7F) - '0';

	//call kexit converting exitValue to integer
	kwakeup(event);
}

//free's up a zombies resources and puts it back in the ready queue
int do_wait()
{
	int pid;
	int status;

	pid = kwait(&status);
	if(pid)
		printf("pid=%d, status=%d\n", pid, status);
	else
		printf("wait error: no child\n");
}

int body()
{ 
	char c;

	printf("proc %d starts from body()\n", running->pid);

	while(1)
	{
		printf("-----------------------------------------\n");
		//prints the list of procs that are initialized and free
		//printf("freelist = ");
		printList("freeList = ", freeList);//show the freeList

		//print ready queue which are procs that are ready
		//printf("readyQueue = ");
		printList(readyQueue); // show the readyQueue

		//printf("sleepList = ");
		printList(sleepList); // show the readyQueue

		printf("proc %d running: parent=%d\n",running->pid,running->ppid);
		printf("enter a char [s|f|q z|a|w] : ");

		c = getc();
		printf("%c\n", c);

		switch(c)
		{
			case 'f': //fork a child off of the current process
				do_kfork();
				break;
			case 's': //switch to next process in ready queue
				do_tswitch();
				break;
			case 'q': //zombie the current process
				do_exit();
				break;
			case 'z': //running PROC go to sleep on an event value
				do_sleep();
				break;
			case 'a': //wakeup all PROCs sleeping on event
				do_wakeup();
				break;
			case 'w': //to wait for a ZOMBIE child
				do_wait();
				break;
		}
	}
}
