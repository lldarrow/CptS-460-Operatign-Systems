/**********************************************
Luke Darrow 11349190
CptS 460
Final Project
grep
***********************************************/

#include "ucode.c"

//grep function
int main(int argc, char *argv[])
{
	int fd, n, i = 0;
	char buf[1024];
	char *token;
	char *pattern;

	printf("doing grep\n");

	if(argc < 3)//no file
	{
		fd = 0;
		//pattern is the second argument
		pattern = argv[1];
	}
	else//there's a file
	{
		//open file for reading
		fd = open(argv[1], O_RDONLY);

		//make sure file opened correctly
		if(fd < 0)
		{
			printf("ERROR: cannot open file\n");
			exit(0);
		}
		//pattern is the third argument
		pattern = argv[2];
	}

	//time to begin the search
	//iterate through the input, one char at a time
	while(read(fd, buf, 1024) > 0)
	{
		//get a line from buffer as determined by the newline char
		token = strtok(buf, "\n");

		//use strstr function to see if the pattern exists in the line
		if(strstr(token, pattern))
			printf("%s\n", token);

		//keep checking each line until no more
		while(token = strtok(0, "\n"))
		{
			if(strstr(token, pattern))
				printf("%s\n", token);
		}
	}

	//close file and exit
	close(fd);
	exit(0);
}
