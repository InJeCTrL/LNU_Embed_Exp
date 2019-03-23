#include<stdio.h>
#include<stdlib.h>

#define BufferSize 1024

int main(int argc, char* args[])
{
	FILE *pFread = NULL, *pFwrite = NULL;
	char temp[BufferSize] = { 0 };
	int len;

	if (argc != 3)
	{
		printf("mycopy [src] [dest]\n");
	}
	else
	{
		pFread = fopen(args[1], "rb");
		pFwrite = fopen(args[2], "wb");
		while (len = fread(temp, 1, BufferSize, pFread))
		{
			fwrite(temp, 1, len, pFwrite);
		}
		fclose(pFread);
		fclose(pFwrite);
		printf("Complete!\n");
	}

	return 0;
}