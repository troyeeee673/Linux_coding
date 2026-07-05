//启用POSIX2008 9 月的标准
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM  3

//自旋锁
pthread_spinlock_t spin;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int n = 0;
void *task(void *arg)
{
    volatile int t = n;
    for(int i = 0 ;i < 10000000; i++)
    {
        /*
        pthread_spin_lock(&spin);
        n ++;
        pthread_spin_unlock(&spin);
        */
       t++;
    }
    pthread_spin_lock(&spin);
    n += t;
    pthread_spin_unlock(&spin);
}

int main()
{
    pthread_spin_init(&spin, 0);

    pthread_t tid[THREAD_NUM];
    for(int i = 0 ;i < THREAD_NUM ;i++)
    {
        pthread_create(&tid[i], NULL, task, NULL);
    }

    for(int i = 0 ;i < THREAD_NUM ;i++)
    {
        pthread_join(tid[i], NULL);
    }

    printf("n = %d", n);
    pthread_spin_destroy(&spin);
    return 0;
}
