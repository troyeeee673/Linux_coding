#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

static int num = 0;

void * handler(void* arg)
{
    for(int i = 0 ;i < 100000 ;i ++)
        num ++;
    return NULL;
}


int main()
{
    pthread_t tid1;
    pthread_t tid2;

    pthread_create(&tid1, NULL, handler, NULL);
    pthread_create(&tid2, NULL, handler, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("%d", num);
    return 0;
}