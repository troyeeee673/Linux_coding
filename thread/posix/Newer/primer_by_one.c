#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *primer(void *n)
{
    unsigned long n2 = *(unsigned long *)n;

    FILE *fp = fopen("primes.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
        return NULL;
    }

    // 先输出唯一的偶质数 2
    if (n2 >= 2)
        fprintf(fp, "2\n");

    // 只检查奇数，步长 2
    for (unsigned long j = 3; j <= n2; j += 2)
    {
        int is_primer = 1;
        // 内层也只检查奇数因子
        for (unsigned long i = 3; i * i <= j; i += 2)
        {
            if (j % i == 0)
            {
                is_primer = 0;
                break;
            }
        }
        if (is_primer)
            fprintf(fp, "%lu\n", j);
    }

    fclose(fp);
    return NULL;
}

int main()
{
    unsigned long n = 100000000UL;
    pthread_t tid;
    pthread_create(&tid, NULL, primer, &n);

    pthread_join(tid, NULL);

    printf("完成，结果已写入 primes.txt\n");
    return 0;
}