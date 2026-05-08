#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define THRNUM      20
#define BUFSIZE     1024

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void* thr_func(void * p)
{
    char linebuf[BUFSIZE];
    FILE* fp;
    fp = fopen("./test.txt", "r+");
    if(fp == NULL)
    {
        perror("open()");
        exit(1);
    }
    pthread_mutex_lock(&mut);
    fgets(linebuf, BUFSIZE, fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", atoi(linebuf)+1);
    fclose(fp);
    pthread_mutex_unlock(&mut);
    pthread_exit(NULL);
}

int main()
{
    int i, j, err;
    pthread_t tid[THRNUM];
    for(i = 0; i < THRNUM; i++)
    {
        err = pthread_create(tid + i, NULL, thr_func, NULL);
        if(err)
        {
            fprintf(stderr, "pthread_create():%s\n", strerror(err));
            for(j = 0; j < i; j++)
                pthread_join(tid[j], NULL);
            exit(1);
        }
    }
    for(j = 0; j < THRNUM; j++)
        pthread_join(tid[j], NULL);
    exit(0);
}
