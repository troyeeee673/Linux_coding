#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_CAPICITY 10
#define PROD_THREAD_NUM 2
#define CONS_THREAD_NUM 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;
int remain_size = MAX_CAPICITY;

// 消费者
void *enter_car(void *arg)
{
    int id = *((int *)arg);
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mutex);
        while (remain_size == 0)
        {
            printf("入口%d:车位满， 阻塞\n", id);
            pthread_cond_wait(&cond_not_full, &mutex);
        }
        remain_size--;
        printf("入口%d: 进入一辆车，剩余车位:%d\n", id, remain_size);
        pthread_cond_signal(&cond_not_empty);
        pthread_mutex_unlock(&mutex);
        sched_yield();
    }

    pthread_exit(NULL);
}

//生产者
void *exit_car(void *arg)
{
    int id = *((int *)arg);
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mutex);
        while(remain_size == MAX_CAPICITY)
        {
            printf("出口%d:车位空， 阻塞\n", id);
            pthread_cond_wait(&cond_not_empty, &mutex);
        }
        remain_size++;
        printf("出口%d: 离开一辆车，剩余车位:%d\n", id, remain_size);
        pthread_cond_signal(&cond_not_full);
        pthread_mutex_unlock(&mutex);
        sched_yield();
    }

    pthread_exit(NULL);
}

int main()
{
    int i;
    pthread_t prod[PROD_THREAD_NUM];
    pthread_t cons[CONS_THREAD_NUM];
    int ids[PROD_THREAD_NUM > CONS_THREAD_NUM ? PROD_THREAD_NUM : CONS_THREAD_NUM];

    for (i = 0; i < CONS_THREAD_NUM; i++)
    {
        ids[i] = i;
        pthread_create(&cons[i], NULL, exit_car, &ids[i]);
    }
    for (i = 0; i < PROD_THREAD_NUM; i++)
    {
        ids[i] = i;
        pthread_create(&prod[i], NULL, enter_car, &ids[i]);
    }


    for (i = 0; i < PROD_THREAD_NUM; i++)
    {
        pthread_join(prod[i], NULL);
    }
    for (i = 0; i < CONS_THREAD_NUM; i++)
    {
        pthread_join(cons[i], NULL);
    }

    exit(0);
}