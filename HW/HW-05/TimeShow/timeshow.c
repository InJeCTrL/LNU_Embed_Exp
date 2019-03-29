#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

#define TRUE 1

int main(void)
{
	time_t t = 0;
	struct tm *LocalTime = NULL, *UTime = NULL;
	
	while (TRUE)
	{
		system("clear");
		time(&t);
		//get local time
		LocalTime = localtime(&t);
		printf("LocalTime: %s", asctime(LocalTime));
		//get GMT
		UTime = gmtime(&t);
		printf("UTime:\t%s", asctime(UTime));
		sleep(1);
	}

	return 0;
}