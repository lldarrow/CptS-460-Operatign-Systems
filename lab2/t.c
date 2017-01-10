/********************************
Luke Darrow 11349190
CptS 460 Lab 2

notes:
tswitch() is an assembly function
where, conceptually, a process
is taken in and another is put out
********************************/

//#include "io.c"

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

	int kstack[SSIZE];		//SSIZE=1024
}PROC;

PROC *get_proc(PROC **list);
int put_proc(PROC **list, PROC *p);
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);

int  procSize = sizeof(PROC);

//running -> current executing PROC
//freeList is a list of FREE PROCS
//readyQueue is a priority of READY PROCs
PROC proc[NPROC], *running = NULL, *freeList = NULL, *readyQueue = NULL;    // define NPROC procs

extern int color;

int body()
{ 
	char c;

	printf("proc %d starts from body()\n", running->pid);

	while(1)
	{
		//prints the list of procs that are initializedd and free
		printQueue(freeList);// optional: show the freeList

		//print ready queue which are procs that are ready
		printQueue(readyQueue); // show the readyQueue

		printf("proc %d running: parent=%d\n",running->pid,running->ppid);
		printf("enter a char [s or f] : ");
		c = getc(); printf("%c\n", c);
		switch(c)
		{
			case 'f': //fork a child off of the current process
				do_kfork();
				break;
			case 's': //switch to next process in ready queue
				do_tswitch();
				break;
			case 'q': //zombie the current process
				my_exit();
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

	printf("init ....\n");

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
	
	/**** create P0 as running ******/
	p = get_proc(&freeList);     // allocate a PROC from freeList
	p->ppid = 0;                 // P0’s parent is itself
	p->status = READY;
	running = p;                 // P0 is now running
}

//as defined in the lab instructions, gets called by tswitch
int scheduler()
{
	if (running->status == READY)		// if running is still READY
		enqueue(&readyQueue, running);	// enter it into readyQueue
	running = dequeue(&readyQueue);		// new running
}

//my code

PROC *get_proc(PROC **list)		//e.g. get_proc(&freeList)
{
	//get a FREE PROC from *list
	
	//return FREE PROC pointer

	//return 0 if no more FREE PROCS

	PROC *my_proc = *list;
	PROC *q = 0; //this will be the list node before p

	// no my_proc! return
	if(!my_proc)
	{
		return my_proc;
	}

	// if my_proc exists and status is not free, then move on to next process until you find a free one
	while (my_proc && my_proc->status != FREE)
	{
		q = my_proc;
		my_proc = my_proc->next;
	}

	if(q == 0)	//if the element we're removing is at the front of the list
	{
		*list = my_proc->next;
		return my_proc;
	}

	//shift over to fill it in
	q->next = my_proc->next;

	//return the process
	return my_proc;
}

//enter p into *list
//lab says to do this but it's not used?
put_proc(PROC **list, PROC *p)
{
	//set the status back to free, since we're putting
	p->status = FREE;

	//if freeList is empty we just set it to p
	//else we put p at the front
	if(freeList == NULL)
	{
		freeList = p;
		p->next = NULL;
	}
	else
	{
		p->next = freeList->next;
		freeList->next = p;
	}
}

//PROC *readyQueue is a priority queue containing PROCs that are READY to run.
//PROCs with the same priority are ordered FIFO

//enter p into queue by priority
enqueue(PROC **queue, PROC *p)
{
	//current and next processes
	PROC *curr, *next;
	
	//if the queue is empty we can set it to p
	if ( (*queue) == NULL )
	{
		*queue = p;
		(*queue)->next = 0;
	}
	else if (p->priority > (*queue)->priority) //the queue is not empty and the proc p has the highest priority
	{
		p->next = (*queue);
		(*queue) = p;
	}
	else //the queue is not empty and proc p has a low priority
	{
		curr = (*queue);
		next = curr->next;

		//keep going until you find something of a lower priority
		while(curr != NULL && p->priority <= next->priority)
		{
			curr = next;
			next = next->next;
		}

		curr->next = p;
		p->next = next;
	}
}

//remove a PROC with the highest priority (the first one in queue) return its pointer
PROC *dequeue(PROC **queue)
{
	//enter p into queue by priority
	PROC *p = *queue;

	if(*queue != NULL)
	{
		*queue = (*queue)->next;
	}

	return p;
}

//print the queue entries in the form of [pid, priority]-> ... ->NULL
printQueue(PROC *queue)
{
	PROC *p;

	//check if queue is empty
	if(queue == NULL)
	{
		printf("ERROR: EMPTY QUEUE\n");
	}
	else
	{
		//print the first process
		//set p to next process in queue
		p = queue->next;
		//print the process id and its priority
		printf("[ %d, %d ]", queue->pid, queue->priority);
		//print the rest of the processes while they remain
		while ( p != NULL )
		{
			printf(" -> [ %d, %d ]", p->pid, p->priority);
			p = p->next;
		}
		//end of the queue displayed as null
		printf(" -> NULL\n");
	}
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

//changes running process status to ZOMBIE, then calls tswitch() to give up CPU
int my_exit()
{
	printf("process [%d] is now a zombie\n", running->pid);
	running->status = ZOMBIE;
	tswitch();
	return 0;
}

//start execution here
main()
{
	printf("MTX starts in main()\n");
	
	init();                 // initialize and create P0 as running
	kfork();                // P0 creates child P1
	
	while(1)				// P0 switches if readyQueue not empty
	{
		if (readyQueue)
			tswitch(); //goes to body
		printf("asdfasdf\n");
	}
	printf("process 0 is still running\n");
}
