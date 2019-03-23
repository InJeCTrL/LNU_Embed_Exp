#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

#define BufferSize 8192

//Compare the two buffers
long DataComp(char *s1, char *s2, long len)
{
	//Offset to the begin
	long ret = 0;	

	while (len--)
	{
		if (*s1 != *s2)
			break;
		s1++, s2++, ret++;
	}
	
	return ret;
}
int main(int argc, char* args[])
{
	int FD1= 0, FD2 = 0;
	char temp1[BufferSize] = { 0 }, temp2[BufferSize] = { 0 };
	int DiffFlag = 0;
	int len, offset, count = 0;

	if (argc != 3)
	{
		printf("mycomp [src1] [src2]\n");
	}
	else
	{
		FD1 = open(args[1], O_RDONLY);
		FD2 = open(args[2], O_RDONLY);
		if (lseek(FD1, 0, SEEK_END) != lseek(FD2, 0, SEEK_END))
		{
			printf("Size doesn't match.\n");
		}
		else
		{
			lseek(FD1, 0, SEEK_SET);
			lseek(FD2, 0, SEEK_SET);
			while (len = read(FD1, temp1, BufferSize))
			{
				read(FD2, temp2, BufferSize);
				//Find difference
				if ((offset = DataComp(temp1, temp2, len)) < len)
				{
					DiffFlag = 1;
					printf("Same Size, but different data.\nOffset:%ld\n", count * BufferSize + offset);
					break;
				}
				count++;//Buffer Page++
			}
			//The same
			if (!DiffFlag)
				printf("Same file.\n");
			
		}
		close(FD1);
		close(FD2);
	}

	return 0;
}