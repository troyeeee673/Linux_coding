// 启用POSIX2008 9 月的标准
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 4
#define THREAD_READ 3
pthread_rwlock_t lock;

int n = 0;

void *myRead(void *arg)
{

    while (1)
    {
        // 加读锁
        pthread_rwlock_rdlock(&lock);
        printf("线程%ld读:%d\n", pthread_self(), n);
        sleep(1);
        pthread_rwlock_unlock(&lock);
        // sleep(1);//释放cpu，让写操作可以进行写入
    }
}

void *myWrite(void *arg)
{
    while (1)
    {
        // 加写锁
        pthread_rwlock_wrlock(&lock);
        n++;
        printf("====线程%ld写:%d====\n", pthread_self(), n);
        sleep(5);
        pthread_rwlock_unlock(&lock);
    }
}
int main()
{
    // 设置读写锁属性
    // 优先等待写者（非递归锁）。
    // 当有写者正在等待时，新来的读者会被阻塞，不允许插队
    // 等当前所有读者释放锁后，写者就能拿到锁
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);

    int i;
    // 加入属性，避免写操作饿死
    pthread_rwlock_init(&lock, &attr);
    pthread_t tid[THREAD_NUM];
    // 创建写线程
    for (i = 0; i < THREAD_NUM - THREAD_READ; i++)
    {
        pthread_create(&tid[i], NULL, myWrite, NULL);
    }

    // 创建读线程
    for (; i < THREAD_NUM; i++)
    {
        pthread_create(&tid[i], NULL, myRead, NULL);
    }

    for (i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_rwlock_destroy(&lock);
    return 0;
}
