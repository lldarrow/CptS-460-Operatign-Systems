
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/

#define PA 13
#define PB 14
#define PC 15
#define PD 16
#define AX  8

char buf[64];

extern u16 loader();

extern PROC proc[];

char *hh[ ] = {"FREE   ", "READY  ", "SLEEP  ", "ZOMBIE ", "KILLED ", 0}; 

int kcinth()
{
   u16 segment, offset;
   int a,b,c,d, r;

//==> WRITE CODE TO GET get syscall parameters a,b,c,d from ustack 

   segment = running->uss;
   offset = running->usp;

   a = get_word(segment, offset + 2*PA);
   b = get_word(segment, offset + 2*PB);
   c = get_word(segment, offset + 2*PC);
   d = get_word(segment, offset + 2*PD);

   switch(a){
       case 0 : r = kgetpid();        break;
       case 1 : r = kps();            break;
       case 2 : r = kchname(b);       break;
       case 3 : r = kkfork();         break;
       case 4 : r = ktswitch();       break;
       case 5 : r = kkwait(b);        break;
       case 6 : r = kkexit(b);        break;
       case 7 : r = getc();           break;
       case 8 : r = putc(b);          break;

       case 99: kkexit(b);            break;
       default: printf("invalid syscall # : %d\n", a); 
   }

//==> WRITE CODE to let r be the return value to Umode
   put_word(r, segment, offset + 2*AX);

}

//============= WRITE C CODE FOR syscall functions ======================

//return the proc's pid
int kgetpid()
{
	return running->pid;
}

//enter Kernel to print the status info of the procs
int kps()
{
    int i, count;
    int showmoreinfo = 0;
    int showevent = 0;
    int showexitcode = 0;
    PROC *p;
    
    printf("*******************************MENU************************************\n");
    printf("*  name       status       pid       ppid       event       exitcode  *\n");
    printf("***********************************************************************\n");

    for (i = 0; i < NPROC; i++)
    {   // initialize all procs
        showmoreinfo = 1;
        showevent = 0;
        showexitcode = 0;
        p = &proc[i];
        count = 14 - strlen(p->name);
        printf("%s", p->name);
        while (count-- > 0)
            printf(" ");
        
        // write the status and set the information vars!
        switch (p->status)
        {
            case FREE:
                printf("FREE         ");
                showmoreinfo = 0;
                showexitcode = 1;
                break;
            case READY:
                printf("READY        ");
                break;
            case RUNNING:
                printf("RUNNING      ");
                break;
            case STOPPED:
                printf("STOPPED      ");
                showexitcode = 1;
                break;
            case SLEEP:
                printf("SLEEP        ");
                showevent = 1;
                break;
            case ZOMBIE:
                printf("ZOMBIE       ");
                showevent = 1;
                break;
        }
        
        // show pid and ppid?
        if (showmoreinfo == 1)
            printf("%d         %d         ", p->pid, p->ppid);
        else
            printf("                    ");
            
        // show event num?
        if (showevent == 1)
            printf("%d             ", p->event);
        else
            printf("              ");
        
        // show exit code?
        if (showexitcode == 1)
            printf("%d\n", p->exitCode);
        else
            printf("\n");
    }
    printf("-----------------------------------------------------------------------\n");
    return 1;
}

//ask for a string *s, enter Kernel to change the running proc's name to *s
int kchname(char *name)
{
	//WRITE C CODE to change running's name string;
	char c;
	int i = 0;
	
	while (i < 32)
	{
		c = get_byte(running->uss, name + i);
		running->name[i] = c;
		if (c == '\0')
			break;
		i++;
	}
}

int kkfork()
{
	//use you kfork() in kernel;
	//return child pid or -1 to Umode!!!
	return kfork("/bin/u1");
}

//enter Kernel to switch process (call tswitch())
int ktswitch()
{
	return tswitch();
}

//enter Kernel to wait for a ZOMBIE chile,
//return its pid (or -1 if no child) and its exitValue
int kkwait(int *status)
{
	int pid, c;
	pid = kwait(&c);
	put_word(c, running->uss, status);
	return pid;
}

//enter Kernel to die with an exitValue
int kkexit(int value)
{
	//use your kexit() in LAB3
	//do NOT let P1 die
	kexit(value);
}
/*
char getc()
{
	char c;
	
	return c;
}

int putc(char c)
{

}
*/
