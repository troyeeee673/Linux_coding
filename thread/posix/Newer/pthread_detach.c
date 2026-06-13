#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *task(void* arg)
{
    int n = *((int *)arg);
    for(int i = 0 ;i <=n ;i ++)
    {
        printf("%ld:%d\n", pthread_self(), i);
    }
    return NULL;
}

int main()
{
    pthread_t tid;

    //线程属性
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int n = 10;
    if(pthread_create(&tid, &attr, task, &n))
    {
        perror("pthread_create()");
        exit(1);
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);

    exit(0);
}