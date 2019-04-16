#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

#define NAME_NAMEDPIPE1 "/tmp/testFIFO_1"
#define NAME_NAMEDPIPE2 "/tmp/testFIFO_2"
#define TRUE 1

int main(void)
{
	int NamedPipe_1, NamedPipe_2;
	int readchnum;
	unsigned char PipeBuf[1024] = { 0 };
	pid_t pid;

	//clear FIFO file
	remove(NAME_NAMEDPIPE1);
	remove(NAME_NAMEDPIPE2);
	//try to create named pipe
	if (mkfifo(NAME_NAMEDPIPE1, 0666) || mkfifo(NAME_NAMEDPIPE2, 0666))
	{
		printf("Create Named Pipe Error!\n");
		return -1;
	}

	pid = fork();
	//fork failed
	if (pid < 0)
	{
		printf("Fork Process Error!\n");
		return -1;
	}
	//parent process
	else if (pid)
	{
		NamedPipe_1 = open(NAME_NAMEDPIPE1, O_RDONLY);
		NamedPipe_2 = open(NAME_NAMEDPIPE2, O_WRONLY);
		while (TRUE)
		{
			printf("This is parent process, say something to child:\n");
			scanf("%s", PipeBuf);
			//send to second pipe
			write(NamedPipe_2, PipeBuf, strlen(PipeBuf));
			//recv from first pipe
			readchnum = read(NamedPipe_1, PipeBuf, 1024);
			PipeBuf[readchnum] = 0;//end of buffer
			//show
			printf("This is parent process, heard from child:\n%s\n\n", PipeBuf);
		}
	}
	//child process
	else
	{
		NamedPipe_1 = open(NAME_NAMEDPIPE1, O_WRONLY);
		NamedPipe_2 = open(NAME_NAMEDPIPE2, O_RDONLY);
		while (TRUE)
		{
			//recv from second pipe
			readchnum = read(NamedPipe_2, PipeBuf, 1024);
			PipeBuf[readchnum] = 0;//end of buffer
			//show
			printf("This is child process, heard from parent:\n%s\n\n", PipeBuf);
			printf("This is child process, say something to parent:\n");
			scanf("%s", PipeBuf);
			//send to first pipe
			write(NamedPipe_1, PipeBuf, strlen(PipeBuf));
		}
	}
	
	return 0;
}