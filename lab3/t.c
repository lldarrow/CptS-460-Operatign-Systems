/********************************
Luke Darrow 11349190
CptS 460 Lab 3

notes:
tswitch() is an assembly function
where, conceptually, a process
is taken in and another is put out
********************************/

#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define RED 
#define NULL 0

//enumerate these based on numbers from lab instructions
#define FREE 0
#define READY 1
#define SLEEP 2
#define BLOCK 3
#define ZOMBIE 4

//the process struct
typedef struct proc{
	struct proc *next;
	int *ksp;
							//  0    1     2     3     4
	int status;				//FREE|READY|SLEEP|BLOCK|ZOMBIE
	int priority;			//priority
	int pid;				//process pid
	int ppid;				//parent pid
	struct proc *parent;	//pointer to parent PROC

	int event;
	int exitCode;

	int kstack[SSIZE];		//SSIZE=1024
}PROC;

int  procSize = sizeof(PROC);

//running -> current executing PROC
//freeList is a list of FREE PROCS
//readyQueue is a priority of READY PROCs
PROC proc[NPROC], *running = NULL, *freeList = NULL, *readyQueue = NULL, *sleepList = NULL;    // define NPROC procs

extern int color;
int nproc = 0;

PROC *kfork();

#include "io.c"
#include "kernel.c"
#include "queue.c"
#include "wait.c"

int body()
{ 
	char c;

	printf("proc %d starts from body()\n", running->pid);

	while(1)
	{
		printf("-----------------------------------------\n");
		//prints the list of procs that are initializedd and free
		printf("freelist = ");
		printQueue(freeList);// optional: show the freeList

		//print ready queue which are procs that are ready
		printf("readyQueue = ");
		printQueue(readyQueue); // show the readyQueue

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

//initializes all the procs
int init()
{
	//placeholder proc and index
	PROC *p;
	int i;

	color = 0x0c;

	printf("init ....");

	for (i=0; i<NPROC; i++)       // initialize all procs
	{
		p = &proc[i];
		p->pid = i;
		p->status = FREE;
		p->priority = 0;
		p->next = &proc[i+1];
	}
	proc[NPROC-1].next = 0;
	freeList = &proc[0];         // all procs are in freeList
	readyQueue = 0;
	sleepList = 0;
	
	/**** create P0 as running ******/
	p = get_proc(&freeList);     // allocate a PROC from freeList
	p->ppid = 0;                 // P0’s parent is itself
	p->status = READY;
	running = p;                 // P0 is now running
	nproc++;
	printf("done\n");
}

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

//as defined in the lab instructions, gets called by tswitch
int scheduler()
{
	if (running->status == READY)		// if running is still READY
		enqueue(&readyQueue, running);	// enter it into readyQueue
	running = dequeue(&readyQueue);		// new running
	color = running->pid + 0x0A;
}

//PROC *readyQueue is a priority queue containing PROCs that are READY to run.
//PROCs with the same priority are ordered FIFO

//as shown in KC's textbook
int do_kfork()
{
	//get pointer to child process from kfork()
	PROC *p = kfork();

	//if 0, then the fork failed!
	if (p == 0)
	{
		printf("ERROR: kfork failed\n"); //error message
		return -1;
	}
	
	//print results
	printf("PROC %d kfork a child %d\n", running->pid, p->pid);
	//return the pid of the newly forked process
	return p->pid;
}

//just calls switch
int do_tswitch()
{
	tswitch();
}

//start execution here
main()
{
	printf("MTX starts in main()\n");
	
	init();                 // initialize and create P0 as running
	kfork();                // P0 creates child P1
	
	while(1)				// P0 switches if readyQueue not empty
	{
		printf("P0 running\n");
		if (nproc == 2 && proc[1].status != READY)
			printf("no runable process, system halts\n");
		while(!readyQueue);
		printf("P0 switch process\n");
		tswitch(); //tswitch to run P1
	}
}
