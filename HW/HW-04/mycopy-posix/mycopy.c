#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

#define BufferSize 1024

int main(int argc, char* args[])
{
	int FDread = 0, FDwrite = 0;
	char temp[BufferSize] = { 0 };
	int len;

	if (argc != 3)
	{
		printf("mycopy [src] [dest]\n");
	}
	else
	{
		FDread = open(args[1], O_RDONLY);
		FDwrite = open(args[2], O_CREAT | O_RDWR, 00644);
		while (len = read(FDread, temp, BufferSize))
		{
			write(FDwrite, temp, len);
		}
		close(FDread);
		close(FDwrite);
		printf("Complete!\n");
	}

	return 0;
}