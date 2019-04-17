#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>

#define TRUE 1

int main(void)
{
    int PID, SIGNO;

    printf("You can send signal to other process here.\n");
    printf("Format:[PID] [SIGNO]\n");
    while (TRUE)
    {
        scanf("%d %d", &PID, &SIGNO);
        kill(PID, SIGNO);
    }

    return 0;
}