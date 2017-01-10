/**********************************************
Luke Darrow 11349190
CptS 460
Final Project
***********************************************/

#include "ucode.c"

//described on page 363 of the book
//argv[0] = login, argv[1] = /dev/ttyX

// etc/passwd format =  username:password:gid:uid:fullname:HOMEDIR:program
//  e.g.    root:xxxxxxx:1000:0:superuser:/root:sh


char *tty;
int in, out, err;//stdin, stdout, stderr

//login function
main(int argc, char *argv[])
{
	char username[64], password[64], buf[1024];
	char *row[64], *line[8];
	char *cp, *cq, *cpp, *cqq;
	int fd; //file descriptor for opening password
	int uid, gid, i = 0, n = 0;

	tty = argv[1];//get port

	close(0); close(1); close(2); //login process may run on different terms

	//open its own tty as stdin, stdout, stderr
	in = open(tty, O_RDONLY);
	out = open(tty, O_WRONLY);
	err = open(tty, O_WRONLY);

	settty(tty); //store tty string in PROC.tty[] for putc()

	//now we can use printf which calls putc() to our tty
	printf("LOGIN : open %s as stdin, stdout, stderr\n", tty);

	signal(2,1); //ignore control-c interrupts so that control-c kills other procs on this tty but not the main sh

	while(1)
	{
		printf("********************************\n");
		//show login to stdout
		printf("username: ");
		//read user name from stdin
		gets(username);

		//show password
		printf("password: ");
		//read user password
		gets(password);
		printf("********************************\n");

		//verify user name and password from /etc/passwd file
		fd = open("/etc/passwd", 0); //open read mode

		//make sure file opened correctly
		if(fd < 0)
		{
			printf("ERROR: cannot open /etc/passwd\n");
			return -1;
		}

		read(fd, buf, 1024);
		n = 0;
		row[n] = strtok(buf, "\n");
		n++;

		//keep adding rows from file
		while(row[n] = strtok(0, "\n"))
		{
			n++;
		}

		//for each row
		for(i = 0; i < n; i++)
		{
			//print rows for debugging
			printf("row = %s\n", row[i]);

			//seperate the values using ':' as a delimiter
			line[0] = strtok(row[i], ":");
			line[1] = strtok(0, ":");
			line[2] = strtok(0, ":");
			line[3] = strtok(0, ":");
			line[4] = strtok(0, ":");
			line[5] = strtok(row, "\n");//last value delimited by newline
			//NOTE: this won't work with a bad passwd file that doesn't use correct format

			//verify username and password
			if(strcmp(username, line[0]) == 0 && strcmp(password, line[1]) == 0)
			{
				//user account valid
				printf("welcome, %s\n", username);

				//set to user uid
				gid = atoi(line[2]);
				uid = atoi(line[3]);

				//changs the ids
				chuid(uid, gid);

				//chdir to user's HOME directory
				chdir(line[5], 0, 0);

				//exec to the program in the user's account
				close(fd);
				printf("exec to /bin/sh ...\n");
				exec("sh");
			}
		}

		//invalid user login, try again
		printf("login failed, try again\n");
		close(fd);//remember to close file!
	}
}
