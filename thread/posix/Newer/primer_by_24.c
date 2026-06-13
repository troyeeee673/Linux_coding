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
    snprintf(filename, sizeof(filename), "primes24_%d.txt", r->thread_id);
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
    pthread_t tid[24];
unsigned long ranges[24][2] = {
    {        3,   4166666},   // 线程1
    {  4166667,   8333333},   // 线程2
    {  8333334,  12500000},   // 线程3
    { 12500001,  16666666},   // 线程4
    { 16666667,  20833333},   // 线程5
    { 20833334,  25000000},   // 线程6
    { 25000001,  29166666},   // 线程7
    { 29166667,  33333333},   // 线程8
    { 33333334,  37500000},   // 线程9
    { 37500001,  41666666},   // 线程10
    { 41666667,  45833333},   // 线程11
    { 45833334,  50000000},   // 线程12
    { 50000001,  54166666},   // 线程13
    { 54166667,  58333333},   // 线程14
    { 58333334,  62500000},   // 线程15
    { 62500001,  66666666},   // 线程16
    { 66666667,  70833333},   // 线程17
    { 70833334,  75000000},   // 线程18
    { 75000001,  79166666},   // 线程19
    { 79166667,  83333333},   // 线程20
    { 83333334,  87500000},   // 线程21
    { 87500001,  91666666},   // 线程22
    { 91666667,  95833333},   // 线程23
    { 95833334, 100000000},   // 线程24
};


    for (int i = 0; i < 24; i++)
    {
        RANGE *r = malloc(sizeof(RANGE));
        r->left     = ranges[i][0];
        r->right    = ranges[i][1];
        r->thread_id = i + 1;
        pthread_create(&tid[i], NULL, primer, r);
    }

    for (int i = 0; i < 24; i++)
        pthread_join(tid[i], NULL);

    printf("完成，结果已写入 primes_1.txt ~ primes_4.txt\n");

    // 合并所有文件
    system("cat primes_*.txt > primes_all.txt");

    return 0;
}