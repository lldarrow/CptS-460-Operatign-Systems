/******************************************
Luke Darrow 11349190
Cpts 460
pipe.c
piping functions
notes on pg. 199-201
******************************************/

//displays the contents of the pipe
show_pipe(PIPE *p)
{
	int i, j;
	printf("------------ PIPE CONTENTS ------------\n"); 
	printf("nreader=%d  nwriter=%d  ", p->nreader, p->nwriter);
	printf("data=%d room=%d\n",p->data, p->room);
	prints("contents=");
	if (p->data)
	{
		j = p->tail;
		for (i=0; i<p->data; i++)
		{
			putc(p->buf[j++]);
			j %= PSIZE;
		}
	}
	printf("\n----------------------------------------\n");
}

char *MODE[] = {"READ","WRITE","RDWR","APPEND","READ_PIPE", "WRITE_PIPE"};//read_pipe and write_pipe are 4 and 5

//print running process' opened file descriptors
int pfd()
{
	int i, count;
	count = 0;

	//count count the opened file descriptors
	for ( i = 0; i < NFD; i++)
	{
		if ( running->fd[i])
			count++;
	}

	//no opened file descriptors
	if(count == 0)
	{
		printf("no running fd's\n");
		return -1;
	}

	//print the running process' opened file descriptors
	printf("========= valid fd =========\n");
	for ( i = 0; i < NFD; i++)
	{
		if (running->fd[i])
			printf("%d    %s  refCount = %d\n", i, MODE[running->fd[i]->mode], running->fd[i]->refCount);
	}
	printf("============================\n");
}


//prompt for a fd and n bytes then try to read nbytes from the pipe to umode. display the bytes read in umode.
int read_pipe(int fd, char *buf, int n)
{
	int r = 0;//number of bytes read so far
	char c;
	OFT *op;
	PIPE *p;

	if(n <= 0)
		return 0;

	//validate fd. from fd, get OFT and pipe pointer p;
	if(fd > NFD || fd < 0 || running->fd[fd] == 0)
	{
		//bad file descriptor, return -1
		printf("bad fd: %d\n", fd);
		return -1;
	}

	op = running->fd[fd];

	//make sure it's in the right mode
	if(op->mode != READ_PIPE)
	{
		printf("fd = %d is NOT for read\n", fd);
		return -1;
	}

	p = op->pipe_ptr;
	printf("pipe before reading\n");
	show_pipe(p);

	while(n)
	{
		//while there's data
		while(p->data)
		{
			//read a byte from pipe to buf
			c = p->buf[p->tail++];
			put_byte(c, running->uss, buf);
			p->tail = p->tail % PSIZE;
			n--;
			r++;
			p->data--;
			p->room++;
			buf++;
			if(n == 0)
				break;
		}
		//has read some data
		if(r || n == 0)
		{
			//wake up writer so you can still write to it
			kwakeup(&p->room);
			printf("pipe after reading\n");
			show_pipe(p);
			return r;
		}
		//pipe has no data
		if(p->nwriter)
		{
			//read nothing, wakes up the writer so you can still write to it
			//sleep the reader
			printf("pipe before reader goes to sleep\n");
			show_pipe(p);
			kwakeup(&p->room);
			ksleep(&p->data);
			continue;
		}

		//pipe has no writer and no data
		return 0;
	}
}


//prompt for a fd and text string, e.g. "abcdefghij", then try to write the text string to the pipe.
int write_pipe(int fd, char *buf, int n)
{
	int r = 0;//number of bytes read so far
	PIPE *p;
	OFT *op;

	if(n <= 0)
		return 0;

	//validate fd. from fd, get OFT and pipe pointer p
	if(fd > NFD || fd < 0 || running->fd[fd] == 0)
	{
		//bad file descriptor, return -1
		printf("bad fd %d\n", fd);
		return -1;
	}

	op = running->fd[fd];

	//make sure it's in the right mode
	if(op->mode != WRITE_PIPE)
	{
		//wrong mode!
		printf("fd = %d is NOT for write\n", fd);
		return -1;
	}

	p = op->pipe_ptr;
	printf("pipe before writing\n");
	show_pipe(p);

	while(n)
	{
		if(!p->nreader)
		{
			//no more readers!
			printf("proc %d : broken pipe error\n", running->pid);
			//exit code from book
			kexit(0x0D);
		}
		while(p->room && n)//while there's room and n is not 0
		{
			//write a byte from buf to pipe
			p->buf[p->head++] = get_byte(running->uss, buf);
			p->head = p->head % PSIZE;
			r++;
			p->data++;
			p->room--;
			n--;
			buf++;
			if(n == 0)
				break;
		}
		//wake up all readers, if any
		kwakeup(&p->data);//

		//wrote n bytes
		if(n == 0)
		{
			printf("pipe after writing\n");
			show_pipe(p);
			return r;
		}

		//still has data to write but pipe has no room
		printf("pipe before writer goes to sleep\n");
		show_pipe(p);

		// still has data to write but pipe has no room
		ksleep(&p->room); //sleep for room
	}
	return r;
}

//create a pipe, fill pd[0] pd[1] (in USER mode!!!) with descriptors
kpipe(int pd[2])
{
	int i;
	PIPE *p;
	OFT *reader, *writer;

	//look for free pipe
	for (i=0; i < NPIPE; i++)
	{
		if (pipe[i].busy == 0)
			break;
	}

	//set pipe to busy
	pipe[i].busy = 1;

	//1. allocate a PIP object. initialize the PIPE object with head=tail=0; data=0; room=PSIZE; nreaders=nwriters=1

	//set p to the pipe index
	p = &pipe[i];

	p->head = 0;
	p->tail = 0;
	p->data = 0;

	p->room = PSIZE;

	p->nreader = 1;
	p->nwriter = 1;

	//2. Allocate 2 OFTs. Initialize the OFTs as readOFT.mode = READ_PIPE and writeOFT.mode = WRITE_PIPE.
	//both OFT's refcount=1 and pipe ptr points to the same PIPE object

	for (i = 0; i < NOFT; i++)
	{
		if (oft[i].refCount == 0)
			break;
	}

	reader = &oft[i];
	reader->refCount = 1;

	for (i = 0; i < NOFT; i++)
	{
		if (oft[i].refCount == 0)
			break;
	}

	writer = &oft[i];
	writer->refCount = 1;

	//set modes to read and write
	reader->mode = READ_PIPE;
	writer->mode = WRITE_PIPE;

	reader->refCount = 1;
	writer->refCount = 1;

	reader->pipe_ptr = p;
	writer->pipe_ptr = p;

	//3. Allocate 2 free entries in the PROC.fd[] array, e.g. fd[i] and fd[j]. Left fd[i] point to readOFT and fd[j] point to writeOFT
	for (i=0; i<NFD; i += 2)
	{ 
		if (running->fd[i] == 0 && running->fd[i+1] == 0)
		{
			running->fd[i] = reader; 
			running->fd[i+1] = writer;
			break;
		}
	}

	//4. write index i to to pd[0] and index i+1 to pd[1] both are in Uspace
	put_word(i, running->uss, pd[0]);
	put_word(i+1, running->uss, pd[1]);
	printf("do_pipe : file descriptors = [%d %d]\n", i, i+1);
	//5. return 0 for OK 
	return 0;
}

//for task to close a fd
int close_pipe(int fd)
{
	OFT *op;
	PIPE *pp;

	printf("proc %d close_pipe: fd=%d\n", running->pid, fd);

	op = running->fd[fd];
	running->fd[fd] = 0; //clear fd[fd] entry

	if(op->mode == READ_PIPE)
	{
		pp = op->pipe_ptr;
		pp->nreader--; // dec n reader by 1

		if (--op->refCount == 0) // last reader
		{
			if (pp->nwriter <= 0) // no more writers
			{
				pp->busy = 0; // free the pipe   
				return;
			}
		}
		kwakeup(&pp->room); 
		return;
	}

	if(op->mode == WRITE_PIPE)
	{
		pp = op->pipe_ptr;
		pp->nwriter--; // dec n reader by 1
		if (--op->refCount == 0) // last writer
		{
			if (pp->nreader <= 0) // no more readers
			{
				pp->busy = 0; //free the pipe
				return;
			}
		}
		kwakeup(&pp->data); //wakeup any WRITER on pipe
		return;
	}
}
