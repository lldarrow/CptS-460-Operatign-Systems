enqueue(PROC **queue, PROC *p)
{
	PROC *last = 0;
	PROC *current = *queue;

	//first find where we need to place
	while((current) && current->priority >= p->priority)
	{
		last = current;
		current = current->next;
	}

	if(!last) //if last is null we are still at front
	{
		if(current)
		{
			p->next = current;
		}
		else
		{
			p->next = 0;
		}
		*queue = p;
	}
	else // else we are in middle
	{
		last->next = p;
		if(current)
		{
			p->next = current;
		}
		else // or at end
		{
			p->next = 0;
		}
	}
}

PROC *dequeue(PROC **queue)
{
    //remove a PROC with the highest priority (the first one in queue)
	PROC* temp = *queue;
	if(temp->next)
	{
		*queue = temp->next;
	}
	else
	{
		*queue = 0;
	}
	return temp;
}

int removeFromQueue(PROC **queue, int pid)
{
	PROC *last = 0;
	PROC *current = *queue;

	if(!current)
	{return 0;}

	if(current->pid == pid)
	{
		*queue = current->next;
		return 1;
	}

	//first find where we need to remove
	while((current->next) && current->pid != pid)
	{
		last = current;
		current = current->next;
	}
	if(current->pid == pid)
	{
		if(last)
		{
			last->next = current->next;
			return 1;
		}
		*queue = 0;
	}
	return 0;
}

printQueue(PROC *queue)
{
    //print the queue entries in the form of [pid, prioirty]-> ... ->NULL
	while(queue)
	{
		printf("[%d,%d]->", queue->pid, queue->priority);
		queue = queue->next;
	}
	printf("NULL\n");
}

printList(char *name, PROC *list)
{
	printf("%s", name);
	printQueue(list);
}


PROC *get_proc(PROC **list)
{
	if(*list)
	{
		return dequeue(list);
	}
	return 0;
}

PROC *put_proc(PROC **list, PROC *p)
{
	PROC *temp = *list;
	p->status = FREE;

	if(!*list)
	{
		*list = p;
		p->next = 0;
	}
	else
	{
		p->next = temp->next;
		temp = p;
	}
}
