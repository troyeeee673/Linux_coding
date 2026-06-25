#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;

#define QUEUE_MAX_SIZE 5 // 队列容量

int size = 0; // 队列当前大小

void *prod(void *arg)
{
    int id = *((int *)arg);
    // 进行10次生产
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mutex);
        while (size == QUEUE_MAX_SIZE)
        {
            printf("生产者%d：队列满，阻塞\n", id);
            pthread_cond_wait(&cond_not_full, &mutex);
        }

        size++;
        printf("生产者%d：size= %d\n", id, size);
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond_not_empty);
        // pthread_cond_signal(&cond_not_empty);
        sched_yield();
    }
    pthread_exit(NULL);
}

void *cons(void * arg)
{
    int id = *((int *)arg);
    // 进行10次消费
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mutex);
        // 这里用while而不是if（只能用于一个生产者、消费者）,因为存在虚假唤醒的问题。
        // 线程调用 pthread_cond_wait 后，即使没有其他线程调用 signal/broadcast，也可能被莫名其妙地唤醒
        // 这是由操作系统的调度机制导致的，无法完全避免
        while (size == 0)
        {
            printf("消费者%d：队列空，阻塞\n", id);
            pthread_cond_wait(&cond_not_empty, &mutex);
        }

        size--;
        printf("消费者%d：size= %d\n", id, size);
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond_not_full);
        // pthread_cond_signal(&cond_not_full);
    }
    sched_yield();
    pthread_exit(NULL);
}

int main()
{
    pthread_t productor[2];
    pthread_t consumer[2];
    int prod_ids[2];
    int cons_ids[2];

    for (int i = 0; i < 2; i++)
    {
        prod_ids[i] = i + 1;
        cons_ids[i] = i + 1;
        pthread_create(&productor[i], NULL, prod, &prod_ids[i]);
        pthread_create(&consumer[i], NULL, cons, &cons_ids[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        pthread_join(productor[i], NULL);
        pthread_join(consumer[i], NULL);
    }

    exit(0);
}