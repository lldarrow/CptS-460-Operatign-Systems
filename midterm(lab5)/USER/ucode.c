/***********************************
Luke Darrow 11349190
Cpts 460
ucode.c
***********************************/

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", "fork", "exec", 0};

#define LEN 64

int show_menu()
{
   printf("***************** Menu ********************************\n");
   printf("*  ps  chname  kfork  switch  wait  exit  fork  exec  *\n");
   printf("*******************************************************\n");
}

//finds command in the array of commands
int find_cmd(char *name)
{
  // return command index
  int i;
  char *p;
  i = 0;
  p = cmd[0];

  while (p){
        if (strcmp(p, name) == 0)
           return i;
        i++;
        p = cmd[i];
  } 
  //command not found, return -1
  return(-1);
}

//returns process id
int getpid()
{
   return syscall(0,0,0);
}

//print processes
int ps()
{
   return syscall(1, 0, 0);
}

//change name of process
int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

//calls kfork
int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
}

//calls kswitch using syscall
int kswitch()
{
    return syscall(4,0,0);
}

//calls wait in int.c using syscall
int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
}

//get integer
int geti()
{
  char s[16];
  return atoi(gets(s));
}

//calls exit in int.c using syscall
int exit()
{
   int exitValue;
   printf("enter an exitValue (0-9): ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   return syscall(6,exitValue,0);
}

//error message for when invalid command inputted, switch table defaults to this
int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}

//calls fork in int.c using syscall
int fork()
{
	//child pid
	int child;
	child = syscall(7, 0, 0, 0);
	if (child)
		printf("parent %d return from fork, child=%d\n", getpid(), child);
	else
		printf("child %d return from fork, child=%d\n", getpid(), child);
}

int exec()
{
	int r;
	char filename[32];
	printf("enter exec filename: ");
	gets(filename);
	r = syscall(8,filename,0,0);
	printf("exec failed\n");
}

/*
char getc()
{
	syscall(8, 0, 0) &0x7F;
}

int putc(char c)
{
	return syscall(9, c, 0, 0);
}
*/
