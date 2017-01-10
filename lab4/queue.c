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
