#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define THRNUM 4

static pthread_mutex_t mut[THRNUM];

static int next(int i)
{
    if(i + 1 == THRNUM)
        return 0;
    return i + 1;
}

static void* abcd_func(void* p)
{
    int n = (intptr_t)p;
    int c = 'a' + n;
    
    while(1)
    {
        pthread_mutex_lock(mut + n);
        write(1, &c, 1);
        pthread_mutex_unlock(mut + next(n));
    }
    pthread_exit(NULL);
}

int main()
{
    int i, j, err;
    pthread_t tid[THRNUM];
    
    for(i = 0; i < THRNUM; i++)
    {
        pthread_mutex_init(mut + i, NULL);
        pthread_mutex_lock(mut + i);
        err = pthread_create(tid + i, NULL, abcd_func, (void*)(intptr_t)i);
        if(err)
        {
            for(j = 0; j < i; j++)
            {
                pthread_mutex_unlock(mut + j);
                pthread_cancel(tid + j);
                pthread_join(tid[j], NULL);
            }
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }
    
    pthread_mutex_unlock(mut + 0);
    alarm(3);
    
    for(i = 0; i < THRNUM; i++)
        pthread_join(tid[i], NULL);
    
    exit(0);
}
