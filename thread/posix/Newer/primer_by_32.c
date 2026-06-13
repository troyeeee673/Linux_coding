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
    snprintf(filename, sizeof(filename), "primes32_%d.txt", r->thread_id);
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
    pthread_t tid[32];
unsigned long ranges[32][2] = {
    {        3,   3125000},   // 线程1
    {  3125001,   6250000},   // 线程2
    {  6250001,   9375000},   // 线程3
    {  9375001,  12500000},   // 线程4
    { 12500001,  15625000},   // 线程5
    { 15625001,  18750000},   // 线程6
    { 18750001,  21875000},   // 线程7
    { 21875001,  25000000},   // 线程8
    { 25000001,  28125000},   // 线程9
    { 28125001,  31250000},   // 线程10
    { 31250001,  34375000},   // 线程11
    { 34375001,  37500000},   // 线程12
    { 37500001,  40625000},   // 线程13
    { 40625001,  43750000},   // 线程14
    { 43750001,  46875000},   // 线程15
    { 46875001,  50000000},   // 线程16
    { 50000001,  53125000},   // 线程17
    { 53125001,  56250000},   // 线程18
    { 56250001,  59375000},   // 线程19
    { 59375001,  62500000},   // 线程20
    { 62500001,  65625000},   // 线程21
    { 65625001,  68750000},   // 线程22
    { 68750001,  71875000},   // 线程23
    { 71875001,  75000000},   // 线程24
    { 75000001,  78125000},   // 线程25
    { 78125001,  81250000},   // 线程26
    { 81250001,  84375000},   // 线程27
    { 84375001,  87500000},   // 线程28
    { 87500001,  90625000},   // 线程29
    { 90625001,  93750000},   // 线程30
    { 93750001,  96875000},   // 线程31
    { 96875001, 100000000},   // 线程32
};


    for (int i = 0; i < 32; i++)
    {
        RANGE *r = malloc(sizeof(RANGE));
        r->left     = ranges[i][0];
        r->right    = ranges[i][1];
        r->thread_id = i + 1;
        pthread_create(&tid[i], NULL, primer, r);
    }

    for (int i = 0; i < 32; i++)
        pthread_join(tid[i], NULL);

    printf("完成，结果已写入 primes_1.txt ~ primes_4.txt\n");

    // 合并所有文件
    system("cat primes_*.txt > primes_all.txt");

    return 0;
}