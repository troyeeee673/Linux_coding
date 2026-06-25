#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

// 互斥锁
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static int num = 0;

void *handler(void *arg)
{
    int t = 0;
    // pthread_mutex_lock(&mutex);
    for (int i = 0; i < 1000000; i++)
    {
        // 如果将锁加到循环内部，会导致加解锁的频繁切换开销远远大于临界区代码的执行开销
        // 对临界区加锁
        //  pthread_mutex_lock(&mutex);
        // 临界区代码
        t ++;
        // num++;
        // pthread_mutex_unlock(&mutex);
    }
    // pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&mutex);
    num += t;
    pthread_mutex_unlock(&mutex);


    return NULL;
}

int main()
{
    // 第二种初始化锁的方法
    //  pthread_mutex_init(&mutex, NULL);

    pthread_t tid1;
    pthread_t tid2;

    pthread_create(&tid1, NULL, handler, NULL);
    pthread_create(&tid2, NULL, handler, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("%d", num);

    // 使用第二种初始化方法，需要手动销毁
    //  pthread_mutex_destroy(&mutex);

    return 0;
}