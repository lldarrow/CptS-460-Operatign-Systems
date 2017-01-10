/***********************************
Luke Darrow 11349190
Cpts 460
ucode.c
***********************************/

char *cmd[]={ "getpid", "ps", "chname", "kfork", "switch", "wait", "exit", "fork", "exec", "pipe", "pfd", "read", "write", "close", "sin", "sout", 0 };

#define LEN 64

int show_menu()
{
   printf("***************** Menu ********************************\n");
   printf("*  ps  chname  kfork  switch  wait  exit  fork  exec  *\n");
   printf("*  pipe  pfd   read   write   close sin   sout        *\n");
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

int pd[2];

int pipe()
{
	printf("pipe syscall\n");
	syscall(30, pd, 0);
	printf("proc %d created a pipe with fd = %d %d\n", getpid(), pd[0], pd[1]);
}

int pfd()
{
	syscall(34, 0, 0, 0);
}

int read_pipe()
{
	char fds[32], buf[1024], bytes[32];
	int fd, n, nbytes;
	pfd();

	printf("enter fd: ");
	gets(fds);

	printf("enter nbytes: ");
	gets(bytes);

	fd = atoi(fds);
	nbytes = atoi(bytes);

	printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);

	n = syscall(31, fd, buf, nbytes);

	if(n >= 0)
	{
		printf("proc %d back to Umode, read %d bytes from pipe : ", getpid(), n);
		buf[n] = 0;
		printf("%s\n", buf);
	}
	else
		printf("read pipe failed\n");
}

int write_pipe()
{
	char fds[16], buf[1024];
	int fd, n, nbytes;
	pfd();

	printf("enter fd: ");
	gets(fds);

	printf("enter text: ");
	gets(buf);

	fd = atoi(fds);
	nbytes = strlen(buf);

	printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);

	n = syscall(32, fd, buf, nbytes);

	if(n >= 0)
		printf("\nproc %d back to Umodde, wrote %d bytes to pipe\n", getpid(), n);
	else
		printf("write pipe failed\n");
}

int close_pipe()
{
	char s[16];
	int fd;
	printf("enter fd to close: ");
	gets(s);
	fd = atoi(s);
	syscall(33, fd);
}

int sout()
{
	syscall(9, "testing, this is a test\r\n");
}

int sin()
{
	char uline[64];
	syscall(10, uline, 0);
	printf("uline=%s\n", uline);
}
