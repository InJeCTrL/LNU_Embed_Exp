#include"threads_def.h"
#include<time.h>
#include<stdio.h>
#include<unistd.h>

void* thread1(void *data)
{
	time_t t = 0;
	struct tm *LocalTime = NULL;
	
	while (1)
	{
		time(&t);
		//get local time
		LocalTime = localtime(&t);
		printf("LocalTime: %s", asctime(LocalTime));
		sleep(1);
	}
}