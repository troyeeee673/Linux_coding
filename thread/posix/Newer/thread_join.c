#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void * task(void* arg)
{
    int *val = malloc(sizeof(int));
    pthread_t tid = pthread_self();

    for(int i = 0 ;i < 10 ;i ++)
    {
        printf("-----------%ld %d\n", tid, i);
        if(i == 5)
        {
            *val = 5;
            pthread_exit((void *)val);
        }
    }


    return NULL;
}

int main()
{
    pthread_t tid;
    if(pthread_create(&tid, NULL, task, NULL))
    {
        perror("pthread_create()");
        exit(1);
    }
    printf("sub: %ld\n", tid);
    printf("main: %ld\n", pthread_self());

    void * retval;
    pthread_join(tid, &retval);
    int p = *((int*)retval);
    printf("%d", p);

    free(retval);

    exit(0);

}