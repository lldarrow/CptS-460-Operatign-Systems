/**********************************************
Luke Darrow 11349190
CptS 460
Final Project
init
***********************************************/

#include "ucode.c"

int console, s0, s1;

//modified from book to do terminals
//basicallly waits for the forked child to die so it can
//restart the login (child dies when user logs out)
void parent()
{
	int pid, status;
	//wait for zombie child
	printf("LDINIT : wait for ZOMBIE child\n");
	while(1)
	{
		pid = wait(&status);//waits for child to die! pid = id of child that died

		//check which child died to login again
		if (pid == console)
		{
			printf("LDINIT: forks a new console login\n");
			console = fork();
			if (console)
				continue;
			else
				exec("login /dev/tty0");
		}
		else if (pid == s0)
		{
			printf("LDINIT: forks a new login on serial port 0\n");
			s0 = fork();
			if (s0)
				continue;
			else
				exec("login /dev/ttyS0");
		}
		else if (pid == s1)
		{
			printf("LDINIT: forks a new login on serial port 1\n");
			s1 = fork();
			if (s1)
				continue;
			else
				exec("login /dev/ttyS1");
		}
		printf("LDINIT : I just buried an orphan child task %d\n", pid);
	}
}

//for some reason this doesn't work with the terminals on my machine
//I'm only able to init on the console BUT I KNOW THE LOGIC!
//basically you take what's in the book and add the serial port logins
int main(int argc, char *argv[ ])
{
	int in, out;

	//set stdin and stdout for console
	in  = open("/dev/tty0", O_RDONLY);
	out = open("/dev/tty0", O_WRONLY);

	printf("%s\n", "LDINIT : fork a login task on console");

	//fork child sh
	console = fork();
	if(console)
	{
		//s1 = fork();
		//if(s1)
		//{
		//	s0 = fork();
		//	if(s0)
		//	{
				parent();
		//	}
		//	else
		//		exec("login /dev/ttyS0");
		//}
		//else
		//	exec("login /dev/ttyS1");
	}
	else
		exec("login /dev/tty0");//login on tty0
}