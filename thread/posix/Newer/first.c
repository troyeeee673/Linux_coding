#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void *thread_task()
{
    for(int k = 0 ; k < 12 ; k++)
    {
        printf("子线程:%d\n", k);
        sleep(1);
    }

}
int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_task, NULL);
    pthread_join(tid, NULL);
    for(int i = 0 ; i < 10; i++)
    {   
        printf("主线程%d\n", i);
        sleep(1);
    }
    // pthread_join(tid, NULL);
    exit(0);
}