#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>

#define TRUE 1

//to deal with signal recv
void CALLBACK_signal(int SIG)
{
    switch (SIG)
    {
        //suspend terminal
        case SIGHUP:
            printf("Catch [SIGHUP] signal!\n");
            break;
        //kbd INT
        case SIGINT:
            printf("Catch [SIGINT] signal!\n");
            break;
        //press quit key
        case SIGQUIT:
            printf("Catch [SIGQUIT] signal!\n");
            break;
        //others
        default:
            printf("Catch other signal!\n");
            break;
    }
    return;
}
int main(void)
{
    printf("Process ID of me is [ %d ]\n", getpid());
    //deal with three signals
    signal(SIGHUP, CALLBACK_signal);
    signal(SIGINT, CALLBACK_signal);
    signal(SIGQUIT, CALLBACK_signal);
    //loop show
    while (TRUE)
    {
        printf("Wait until catch a signal!\n");
        pause();
    }

    return 0;
}