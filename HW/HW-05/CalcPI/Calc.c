#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

#define TRUE 1
#define MAX_ACCU 500

int main(void)
{
	int R[MAX_ACCU] = { 2 };//R(1) = 2.00...
	int Sum[MAX_ACCU] = { 2 };//Sum(1) = 2.00...
	long i, j, t, d;
	int plus;
	struct timeval tv1, tv2;

	gettimeofday(&tv1, NULL);//get start time
	//Calc
	for (i = 1; i < MAX_ACCU; i++)
	{
		//R(n)*(n)
		plus = 0;
		for (j = MAX_ACCU - 1; j >= 0; j--)
		{
			t = R[j] * i + plus;
			R[j] = t % 10;
			plus = t / 10;
		}
		//R(n+1) = R(n)*n / (2*n+1)
		d = 2 * i + 1;
		plus = 0;
		for (j = 0; j < MAX_ACCU; j++)
		{
			t = R[j] + plus * 10;
			R[j] = t / d;
			plus = t % d;
		}
		//Sum(n+1) = Sum(n) + R(n+1)
		plus = 0;
		for (j = MAX_ACCU - 1; j >= 0; j--)
		{
			t = Sum[j] + R[j] + plus;
			Sum[j] = t % 10;
			plus = t / 10;
		}
	}
	gettimeofday(&tv2, NULL);//get stop time
	//Output
	printf("time: %d us\n", tv2.tv_sec * 1000000 + tv2.tv_usec - tv1.tv_sec * 1000000 - tv1.tv_usec);
	printf("%d.", Sum[0]);
	for (i = 1; i < MAX_ACCU; i++)
	{
		printf("%d", Sum[i]);
	}
	printf("\n");

	return 0;
}
