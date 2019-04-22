#include"threads_def.h"
#include<pthread.h>

int main(void)
{
    pthread_t thread_1, thread_2;
    void *ret = NULL;

    pthread_create(&thread_1, NULL, thread1, 0);
    pthread_create(&thread_2, NULL, thread2, 0);

    pthread_join(thread_1, &ret);
    pthread_join(thread_1, &ret);
    return 0;
}