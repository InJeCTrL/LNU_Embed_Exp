/*
    Single Consumer
    Execute Producer First to initialize semaphore!
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<time.h>

#define ID_SEM_FULL 666
#define ID_SEM_EMPTY 777
#define ID_SEM_MUTEX 888
#define NUM_SEM 5   //5 boxes to be used
#define TRUE 1

//P
int Wait(int SemID)
{
    struct sembuf sem;

    sem.sem_num = 0;
    sem.sem_op = -1;
    sem.sem_flg = SEM_UNDO;
    semop(SemID, &sem, 1);

    return 0;
}
//V
int Signal(int SemID)
{
    struct sembuf sem;

    sem.sem_num = 0;
    sem.sem_op = 1;
    sem.sem_flg = SEM_UNDO;
    semop(SemID, &sem, 1);

    return 0;
}
int main(void)
{
    int SemID_Full, SemID_Empty, SemID_Mutex;
    time_t t;
    struct tm *LocalTime = NULL;

    //try to create or get semaphore
    SemID_Full = semget(ID_SEM_FULL, 1, 0666 | IPC_CREAT);
    SemID_Empty = semget(ID_SEM_EMPTY, 1, 0666 | IPC_CREAT);
    SemID_Mutex = semget(ID_SEM_MUTEX, 1, 0666 | IPC_CREAT);
    if (SemID_Full == -1 || SemID_Empty == -1 || SemID_Mutex == -1)
    {
        printf("Create / Get Semaphore failed!\n");
        return -1;
    }
    printf("Get Semaphore!\n");
    //consume loop
    while (TRUE)
    {
        time(&t);
        Wait(SemID_Mutex);
        Wait(SemID_Empty);
        printf("%s - Get\n", asctime(localtime(&t)));
        Signal(SemID_Full);
        Signal(SemID_Mutex);
        sleep(1);
    }

    return 0;
}