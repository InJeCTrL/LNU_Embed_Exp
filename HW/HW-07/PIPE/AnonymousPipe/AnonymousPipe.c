#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>

#define TRUE 1

int main(void)
{
	int AnonymousPipe_1[2] = { 0 }, AnonymousPipe_2[2] = { 0 };
	int readchnum;
	unsigned char PipeBuf[1024] = { 0 };
	pid_t pid;

	//try to create anonymous pipe
	if (pipe(AnonymousPipe_1) < 0 || pipe(AnonymousPipe_2) < 0)
	{
		printf("Create Anonymous Pipe Error!\n");
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
		close(AnonymousPipe_1[0]);
		while (TRUE)
		{
			printf("This is parent process, say something to child:\n");
			scanf("%s", PipeBuf);
			//send to first pipe
			write(AnonymousPipe_1[1], PipeBuf, strlen(PipeBuf));
			//recv from second pipe
			readchnum = read(AnonymousPipe_2[0], PipeBuf, 1024);
			PipeBuf[readchnum] = 0;//end of buffer
			//show
			printf("This is parent process, heard from child:\n%s\n\n", PipeBuf);
		}
	}
	//child process
	else
	{
		close(AnonymousPipe_2[0]);
		while (TRUE)
		{
			//recv from first pipe
			readchnum = read(AnonymousPipe_1[0], PipeBuf, 1024);
			PipeBuf[readchnum] = 0;//end of buffer
			//show
			printf("This is child process, heard from parent:\n%s\n\n", PipeBuf);
			printf("This is child process, say something to parent:\n");
			scanf("%s", PipeBuf);
			//send to second pipe
			write(AnonymousPipe_2[1], PipeBuf, strlen(PipeBuf));
		}
	}
	
	return 0;
}