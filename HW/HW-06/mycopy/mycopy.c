#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("mycopy [src] [dest]\n");
	}
	else
	{
		execl("/bin/cp", "cp", argv[1], argv[2], NULL);
	}

	return 0;
}