#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define THREA_NUM 3
#define TICKET_COUNT 100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int tickets = TICKET_COUNT;

void *sale_tickets(void *arg)
{
    pthread_mutex_lock(&mutex);
    while (1)
    {
        if (tickets <= 0)
            break;
        printf("窗口%d售出第%d张票\n", *((int *)arg), tickets--);
        // sleep(1);
        pthread_mutex_unlock(&mutex);
        sched_yield();
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[THREA_NUM];
    int i;
    int ids[i];

    for (i = 0; i < THREA_NUM; i++)
    {
        ids[i] = i;
        pthread_create(&tid[i], NULL, sale_tickets, &ids[i]);
    }
    for (i = 0; i < THREA_NUM; i++)
    {
        pthread_join(tid[i], NULL);
    }
    exit(0);
}