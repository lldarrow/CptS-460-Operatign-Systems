/**********************************************
Luke Darrow 11349190
CptS 460
Final Project
cat
***********************************************/

#include "ucode.c"

//cat function
int main(int argc, char *argv[])
{
	int fd, n, i, terminal;
	char c, c2, buf[1024], carriage = '\r';
	char *tty;

	//printing ruins this when it's use as input for another command
	//printf("RUNNING LUKE'S CAT\n");

	//echo back until ctrl-d to kill the process
	if(argc < 2)
	{
		//set fd to stdin
		fd = 0;

		//iterate through the input, one char at a time
		while(read(fd, &c, 1) > 0)
		{
			//gettty(tty);//get proc.tty name, stores fd to terminal to redirect to terminal
			//terminal = open(tty, O_WRONLY);

			//c2 = c;

			//handle carriage returns and new lines
			if(c == carriage)
			{
				putc('\n');
				//write(terminal, "\n", 1);
				//write(terminal, "\r", 1);
			}
			else if(c == '\n')
			{
				putc('\r');
				putc('\n');
			}
			else//no carriage returns or symbols, output the character
			{
				putc(c);
				//write(terminal, &c2, 1);
			}
			//close(terminal);
		}
	}
	else
	{
		//open file for reading
		fd = open(argv[1], O_RDONLY);

		//check if opened correctly
		if(fd < 0)
		{
			printf("ERROR: cannot open file\n");
			exit(0);
		}

		//keep reading 1024 bytes at a time (n is the number of bytes actually read) into buf
		while( (n = read(fd, buf, 1024)) > 0)
		{
			//for every byte
			for(i = 0; i < n; i++)
			{
				//write to stdin
				write(1, &(buf[i]), 1);

				//if newline character
				if(buf[i] == '\n')
					write(2, &carriage, 1);//write carriage to stdout
			}

			//clear the buffer since if we don't read enough to overwrite everything, there will be problems!
			for(i = 0; i < 1024; i++)
				buf[i] = 0;
		}
	}

	//close file descriptor and kill the process
	close(fd);
	exit(0);
}