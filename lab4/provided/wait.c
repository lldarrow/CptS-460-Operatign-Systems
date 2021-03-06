/*************************************
Luke Darrow 11349190
CptS 460 Lab 4 wait.c
*************************************/

//stops process to wait for an event
int ksleep(int event)
{
	running->event = event;  // record event in PROC.event
	running->status = SLEEP; // change status to SLEEP
	tswitch();               // give up CPU, switch process
}

//wake up sleeping process for an event
int kwakeup(int event)
{
	int i;
	PROC *p;

	//for every process check if asleep and event
	for (i=1; i<NPROC; i++) // not applicable to P0
	{
		p = &proc[i];
		if (p->status == SLEEP && p->event == event)
		{
			p->event = 0;      // cancel PROC’s event
			p->status = READY; // make it ready to run again
			enqueue(&readyQueue, p);
		}
	}
}

int ready(PROC *p)
{
	p->status=READY;
	enqueue(&readyQueue, p);
}

//turns process into zombie. if it has children, all its children go to p1
int kexit(int exitValue)
{
	int i;
	int wakeupP1 = 0;
	PROC *p = &proc[i];

	if (running->pid==1 && nproc>2) // nproc = number of active PROCs
	{
		printf("other procs still exist, P1 can't die yet\n");
		return -1;
	}

	/* send children (dead or alive) to P1's orphanage */
	for (i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		if (p->status != FREE && p->ppid == running->pid)
		{
			//send the killed process' children to parent process 1
			p->ppid = 1;
			p->parent = &proc[1];
			wakeupP1++;
		}
	}

	/* record exitValue and become a ZOMBIE */
	running->exitCode = exitValue;
	running->status = ZOMBIE;

	/* wakeup parent and also P1 if necessary */
	kwakeup(running->parent); // parent sleeps on its PROC address
	if (wakeupP1)
		kwakeup(&proc[1]);
	tswitch();                // give up CPU
}

//free's up a zombies resources and puts it back in the ready queue
int kwait(int *status)  // wait for ZOMBIE child
{
	PROC *p;
	int i, hasChild = 0;

	while(1) // search PROCs for a child
	{
		//for each process, check it
		for (i=1; i<NPROC; i++) // exclude P0
		{
			p = &proc[i];
			//if the process is free and the process is running...
			if (p->status != FREE && p->ppid == running->pid)
			{
				hasChild = 1;                // has child flag
				if (p->status == ZOMBIE) // lay the dead child to rest
				{
					*status = p->exitCode;    // collect its exitCode
					p->status = FREE;        // free its PROC 
					put_proc(&freeList, p);  // to freeList
					nproc--;                 // once less processes
					return(p->pid);          // return its pid
				}
			}
		}
		if (!hasChild)
			return -1;          // no child, return ERROR 
		ksleep(running); // still has kids alive: sleep on PROC address
	}
}
