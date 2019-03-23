#include<unistd.h>
#include<stdio.h>
#include "funcs.h"
int main(void)
{
	//Test 1.2+2.3 == 3.5
	printf("%f\n", adder(1.2,2.3));
	//Test 5.3-2.1 == 3.2
	printf("%f\n", minus(5.3,2.1));

	return 0;
}