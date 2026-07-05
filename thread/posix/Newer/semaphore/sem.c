#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 8
#define N   4

sem_t sem;

void *task(void* arg)
{
    int id = *((int*)arg);
    free(arg);

    //此时8个线程不会同时打印启动语句了，因为打印前要获取信号量
    sem_wait(&sem);
    printf("线程%d启动\n", id);
    sleep(3);
    printf("线程%d结束\n", id);
    sem_post(&sem);

    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[MAX];
    sem_init(&sem, 0, N);
    for(int i = 0 ;i < MAX; i++)
    {
        int *id = (int *)malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&tid[i], NULL, task, id);
    }


    for(int i = 0 ;i < MAX ;i ++)
    {
        pthread_join(tid[i], NULL);
    }
    sem_destroy(&sem);

    return 0;
}

