// #include <stdio.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <stdlib.h>

// #define SIZE_OF_SEM  2
// #define SIZE_OF_PROD 3
// #define SIZE_OF_CONS 3

// sem_t sem;

// // 生产者
// void *productor(void *arg)
// {
//     int id = *((int *)arg);
//     for (int i = 0; i < 10; i++)
//     {
//         printf("生产者%d:第%d轮\n", id, i);
//         sem_post(&sem);
//         sched_yield();
//     }
// }

// // 消费者
// void *consumer(void *arg)
// {
//     int id = *((int *)arg);
//     for (int i = 0; i < 10; i++)
//     {
//         printf("消费者%d:第%d轮\n", id, i);
//         sem_wait(&sem);
//         sched_yield();
//     }
// }

// int main()
// {
//     int i;
//     pthread_t prod[SIZE_OF_PROD];
//     pthread_t cons[SIZE_OF_CONS];
//     int ids[SIZE_OF_CONS > SIZE_OF_PROD ? SIZE_OF_CONS : SIZE_OF_PROD];

//     sem_init(&sem, 0, SIZE_OF_SEM);

//     for (i = 0; i < SIZE_OF_PROD; i++)
//     {
//         ids[i] = i;
//         pthread_create(&prod[i], NULL, productor, &ids[i]);
//     }
//     for (i = 0; i < SIZE_OF_CONS; i++)
//     {
//         ids[i] = i;
//         pthread_create(&cons[i], NULL, consumer, &ids[i]);
//     }

//     for (i = 0; i < SIZE_OF_PROD; i++)
//     {
//         pthread_join(prod[i], NULL);
//     }
//     for (i = 0; i < SIZE_OF_CONS; i++)
//     {
//         pthread_join(cons[i], NULL);
//     }

//     sem_destroy(&sem);
//     exit(0);
// }

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define SIZE_OF_SEM  2
#define SIZE_OF_PROD 3
#define SIZE_OF_CONS 3
#define TOTAL_ITEMS  30  // 总共需要生产和消费的数量

sem_t sem_empty;   // 空槽位数
sem_t sem_full;    // 已填充槽位数
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int produced_count = 0;  // 已生产数量
int consumed_count = 0;  // 已消费数量

void *productor(void *arg)
{
    int id = *((int *)arg);
    for (int i = 0; i < 10; i++)  // 每个生产者生产10个
    {
        sem_wait(&sem_empty);  // 先等待空槽位
        pthread_mutex_lock(&mutex);
        
        produced_count++;
        printf("生产者%d: 生产第%d个产品\n", id, produced_count);
        
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full);  // 通知消费者
        
        usleep(rand() % 100000);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg)
{
    int id = *((int *)arg);
    for (int i = 0; i < 10; i++)  // 每个消费者消费10个
    {
        sem_wait(&sem_full);  // 先等待产品
        pthread_mutex_lock(&mutex);
        
        consumed_count++;
        printf("消费者%d: 消费第%d个产品\n", id, consumed_count);
        
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_empty);  // 释放空槽位
        
        usleep(rand() % 200000);
    }
    pthread_exit(NULL);
}

int main()
{
    int i;
    pthread_t prod[SIZE_OF_PROD];
    pthread_t cons[SIZE_OF_CONS];
    
    // 为每个线程分配独立的ID
    int prod_ids[SIZE_OF_PROD];
    int cons_ids[SIZE_OF_CONS];
    
    // 初始化信号量
    sem_init(&sem_empty, 0, SIZE_OF_SEM);  // 初始有SIZE_OF_SEM个空槽位
    sem_init(&sem_full, 0, 0);             // 初始没有产品
    
    srand(time(NULL));
    
    // 创建线程
    for (i = 0; i < SIZE_OF_PROD; i++)
    {
        prod_ids[i] = i + 1;
        pthread_create(&prod[i], NULL, productor, &prod_ids[i]);
    }
    for (i = 0; i < SIZE_OF_CONS; i++)
    {
        cons_ids[i] = i + 1;
        pthread_create(&cons[i], NULL, consumer, &cons_ids[i]);
    }
    
    // 等待线程结束
    for (i = 0; i < SIZE_OF_PROD; i++)
    {
        pthread_join(prod[i], NULL);
    }
    for (i = 0; i < SIZE_OF_CONS; i++)
    {
        pthread_join(cons[i], NULL);
    }
    
    printf("所有生产和消费完成！\n");
    printf("总共生产: %d, 总共消费: %d\n", produced_count, consumed_count);
    
    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}