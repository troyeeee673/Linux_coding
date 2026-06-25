#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

sem_t sem;
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int count = 0;

void * task(void*arg)
{
    for(int i = 0 ;i < 100000; i++)
    {
        // pthread_mutex_lock(&mutex);
        sem_wait(&sem);//V
        count ++;
        // pthread_mutex_unlock(&mutex);
        sem_post(&sem);//P
    }

    pthread_exit(NULL);
}

int main()
{
    /*
    *初始化信号量
    *@param:sem     信号量
    *@param:0       （一个进程中）线程间的信号量
    *@param:1       信号量的值 = 1表示二元信号量
    **/
    sem_init(&sem, 0, 1);

    pthread_t tid1;
    pthread_t tid2;

    pthread_create(&tid1, NULL, task, NULL);
    pthread_create(&tid2, NULL, task, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("%d\n", count);

    sem_destroy(&sem);

    exit(0);

}