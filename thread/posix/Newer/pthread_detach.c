#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//分离的子线程，特点是子线程后台执行，不需要返回数据，不需要进行join,自动进行资源释放
//缺点是无法返回数据

void *task(void* arg)
{
    int n = *((int *)arg);
    for(int i = 0 ;i <=n ;i ++)
    {
        printf("%ld:%d\n", pthread_self(), i);
    }
    return NULL;
}

int main()
{
    pthread_t tid;

    //线程属性
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int n = 10;
    if(pthread_create(&tid, &attr, task, &n))
    {
        perror("pthread_create()");
        exit(1);
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);

    exit(0);
}