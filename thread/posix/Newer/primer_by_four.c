#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct
{
    unsigned long left;
    unsigned long right;
    int thread_id;          // 用于生成独立文件名
} RANGE;

void *primer(void *range)
{
    RANGE *r = (RANGE*)range;
    unsigned long left  = r->left;
    unsigned long right = r->right;

    // 每个线程写自己的文件
    char filename[64];
    snprintf(filename, sizeof(filename), "primes_%d.txt", r->thread_id);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("fopen");
        return NULL;
    }

    // 跳过偶数
    if (left <= 2 && right >= 2)
        fprintf(fp, "2\n");
    if (left % 2 == 0)
        left++;

    for (unsigned long j = left; j <= right; j += 2)
    {
        int is_primer = 1;
        for (unsigned long i = 3; i * i <= j; i += 2)
        {
            if (j % i == 0)
            {
                is_primer = 0;
                break;
            }
        }
        if (is_primer)
        {
            fprintf(fp, "%lu\n", j);
        }
    }

    fclose(fp);
    free(r);
    return NULL;
}

int main()
{
    pthread_t tid[4];

    unsigned long ranges[4][2] = {
        {3,          25000000},
        {25000001,   50000000},
        {50000001,   75000000},
        {75000001,  100000000}
    };

    for (int i = 0; i < 4; i++)
    {
        RANGE *r = malloc(sizeof(RANGE));
        r->left     = ranges[i][0];
        r->right    = ranges[i][1];
        r->thread_id = i + 1;
        pthread_create(&tid[i], NULL, primer, r);
    }

    for (int i = 0; i < 4; i++)
        pthread_join(tid[i], NULL);

    printf("完成，结果已写入 primes_1.txt ~ primes_4.txt\n");

    // 合并所有文件
    system("cat primes_*.txt > primes_all.txt");

    return 0;
}