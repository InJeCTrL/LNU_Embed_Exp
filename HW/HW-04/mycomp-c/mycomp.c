#include<stdio.h>
#include<stdlib.h>

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
	FILE *pF1= NULL, *pF2 = NULL;
	char temp1[BufferSize] = { 0 }, temp2[BufferSize] = { 0 };
	int DiffFlag = 0;
	int len, offset, count = 0;

	if (argc != 3)
	{
		printf("mycomp [src1] [src2]\n");
	}
	else
	{
		pF1 = fopen(args[1], "rb");
		pF2 = fopen(args[2], "rb");
		fseek(pF1, 0, SEEK_END);
		fseek(pF2, 0, SEEK_END);
		if (ftell(pF1) != ftell(pF2))
		{
			printf("Size doesn't match.\n");
		}
		else
		{
			fseek(pF1, 0, SEEK_SET);
			fseek(pF2, 0, SEEK_SET);
			while (len = fread(temp1, 1, BufferSize, pF1))
			{
				fread(temp2, 1, BufferSize, pF2);
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
		fclose(pF1);
		fclose(pF2);
	}

	return 0;
}