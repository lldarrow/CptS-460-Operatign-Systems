
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
   //WRITE C code to print PROC information
   int i,j; 
   char *p, *q, buf[16];
   buf[15] = 0;

   printf("============================================\n");
   printf("  name         status      pid       ppid  \n");
   printf("--------------------------------------------\n");

   for (i=0; i<NPROC; i++){
       strcpy(buf,"               ");
       p = proc[i].name;
       j = 0;
       while (*p){
             buf[j] = *p; j++; p++;
       }      
       prints(buf);    prints(" ");
       
       if (proc[i].status != FREE){
           if (running==&proc[i])
              prints("running");
           else
              prints(hh[proc[i].status]);
           prints("     ");
           printf("%d", proc[i].pid);
           prints("        ");
           printf("%d", proc[i].ppid);
       }
       else{
              prints("FREE");
       }
       printf("\n");
   }
   printf("---------------------------------------------\n");

   return(0);
}

//ask for a string *s, enter Kernel to change the running proc's name to *s
int kchname(char *name)
{
  //WRITE C CODE to change running's name string;
  char *cp = buf;
  int count = 0; 
  while (count < 32){
     *cp = get_byte(running->uss, name);
     if (*cp == 0) break;
     cp++;
     name++;
     count++;
  }
  buf[31] = 0;

  printf("changing name of proc %d to %s\n", running->pid, buf);
  strcpy(running->name, buf); 
  printf("done\n");
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
