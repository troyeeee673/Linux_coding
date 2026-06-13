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
    snprintf(filename, sizeof(filename), "primes16_%d.txt", r->thread_id);
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
    pthread_t tid[16];

unsigned long ranges[16][2] = {
    {        3,   6250000},   // 线程1
    {  6250001,  12500000},   // 线程2
    { 12500001,  18750000},   // 线程3
    { 18750001,  25000000},   // 线程4
    { 25000001,  31250000},   // 线程5
    { 31250001,  37500000},   // 线程6
    { 37500001,  43750000},   // 线程7
    { 43750001,  50000000},   // 线程8
    { 50000001,  56250000},   // 线程9
    { 56250001,  62500000},   // 线程10
    { 62500001,  68750000},   // 线程11
    { 68750001,  75000000},   // 线程12
    { 75000001,  81250000},   // 线程13
    { 81250001,  87500000},   // 线程14
    { 87500001,  93750000},   // 线程15
    { 93750001, 100000000},   // 线程16
};


    for (int i = 0; i < 16; i++)
    {
        RANGE *r = malloc(sizeof(RANGE));
        r->left     = ranges[i][0];
        r->right    = ranges[i][1];
        r->thread_id = i + 1;
        pthread_create(&tid[i], NULL, primer, r);
    }

    for (int i = 0; i < 16; i++)
        pthread_join(tid[i], NULL);

    printf("完成，结果已写入 primes_1.txt ~ primes_4.txt\n");

    // 合并所有文件
    system("cat primes_*.txt > primes_all.txt");

    return 0;
}