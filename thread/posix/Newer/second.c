#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int count = 0;

void primer(int range1, int range2)
{

    int n;
    int limit = (int)sqrt(range2);
    int i;

    while(1)
    {
        pthread_mutex_lock(&lock);
        n = range1 + count;
        count++;        

        pthread_mutex_unlock(&lock);
        if (n > range2)
            break;
        int is_primer = 1;
        for (i = 2; i * i <= n; i++)
        {
            if (n % i == 0)
            {
                is_primer = 0;
                break;
            }
        }
        if(is_primer)
            printf("%d\n", n);
    }
}



void *task1(void *arg)
{
    // int *n = (int*)arg;
    int n = *((int *)arg);
    pthread_t tid = pthread_self();
    printf("线程1\n");
    primer(2, n);
    sleep(1);
}

void *task2(void *arg)
{
    int n = *((int *)arg);
    pthread_t tid = pthread_self();    
    printf("线程2\n");
    primer(2, n);

    sleep(5);
}

void *task3(void *arg)
{
    int n = *((int *)arg);
    pthread_t tid = pthread_self();    
    printf("线程3\n");
    primer(2, n);
    sleep(1);
}

void *task4(void *arg)
{
    int n = *((int *)arg);
    pthread_t tid = pthread_self();    
    printf("线程4\n");
    primer(2, n);

    sleep(1);
}

int main()
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_t tid4;

    // 参数：
    // 基本数据
    // 复合类型（结构体、联合体）
    int n = 1000;
    pthread_create(&tid1, NULL, task1, &n);
    pthread_create(&tid2, NULL, task2, &n);
    pthread_create(&tid3, NULL, task3, &n);
    pthread_create(&tid4, NULL, task4, &n);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    exit(0);
}