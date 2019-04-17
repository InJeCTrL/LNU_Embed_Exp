/*
    Single Producer
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
    semctl(SemID_Full, 0, SETVAL, NUM_SEM);//Full = 5
    semctl(SemID_Empty, 0, SETVAL, 0);//Empty = 0
    semctl(SemID_Mutex, 0, SETVAL, 1);//Mutex = 1
    printf("Semaphore initialized!\n");
    //produce loop
    while (TRUE)
    {
        time(&t);
        printf("%s - Produce\n", asctime(localtime(&t)));
        Wait(SemID_Mutex);
        Wait(SemID_Full);
        time(&t);
        printf("%s - Put\n", asctime(localtime(&t)));
        Signal(SemID_Empty);
        Signal(SemID_Mutex);
        sleep(1);
    }

    return 0;
}