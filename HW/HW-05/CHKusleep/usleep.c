#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

#define DelayTime 10000

int main(void)
{
	struct timeval tv1, tv2;
	int Delta;

	gettimeofday(&tv1, NULL);
	usleep(DelayTime);
	gettimeofday(&tv2, NULL);
	Delta = tv2.tv_sec * 1000000 + tv2.tv_usec - tv1.tv_sec * 1000000 - tv1.tv_usec - DelayTime;
	printf("DelayTime = %d\nDelta = %ld\nDelta(Percent) = %.2f\n", DelayTime, Delta, (Delta * 100.0) / DelayTime);

	return 0;
}