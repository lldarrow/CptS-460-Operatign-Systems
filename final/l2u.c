/**********************************************
Luke Darrow 11349190
CptS 460
Final Project
lower to uppercase

if no arguments, outputs uppercase letters
to stdout. Else, outputs uppercase translation
of file one to file two.
***********************************************/

#include "ucode.c"

//l2u function
int main(int argc, char *argv[ ])
{  
	int fd1, fd2, n, c;
	char buf[1024], *cp;

	//no file, stdin to out
	if (argc == 1)
	{
		//while read one byte into c
		while( read(0, &c, 1) > 0)
		{
			//if no uppercase, make it uppercase. else, just print it!
			if ( c >= 'a' && c <= 'z')
				putc(c - 'a' + 'A');
			else
				putc(c);

			//if carriage return, stdout a new line
			if (c == '\r')
				putc('\n');
		}
		//done! kill the process
		exit(0);
	}

	//other usage of lower to upper
	if (argc < 3)
	{
		printf("ERROR: too few arguments\n");
		printf("e.g. l2u f1 f2\n");
		exit(1);
	}

	//open file one for reading
	fd1 = open(argv[1], O_RDONLY);

	//check if opened correctly
	if (fd1 < 0)
	{
		printf("ERROR: cannot open file %s\n", argv[1]);
		exit(1);
	}

	//open file two for writing or create it for writing
	fd2 = open(argv[2], O_WRONLY|O_CREAT);

	//keep reading file one and outputing to file two
	while( n = read(fd1, buf, 1024) )
	{
		//set pointer to start of buf
		cp = buf;
		//go through buf and shift to uppercase byte by byte
		while(cp < buf + n)
		{
			//if within lowercase range, convert to uppercase
			if (*cp >= 'a' && *cp <= 'z')
			{
				*cp = *cp - 'a' + 'A';
			}
			//increment pointer through buffer
			cp++;
		}
		//write the modified buffer back to the second file
		write(fd2, buf, n);
	}
	//done!
	printf("l2u complete!\n");
	exit(0);
}
