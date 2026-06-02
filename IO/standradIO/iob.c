//使用二进制格式进行读写
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "iob.h"

int main(int argc, char** argv)
{
    struct person p[4];
    struct person p1 = {"Cassie", 21};
    struct person p2;
    strcpy(p2.name, "Sid");
    p2.age = 21;
    struct person *p3;
    p3 = malloc(sizeof(struct person));
    p3->age = 21;
    strcpy(p3->name, "Effy");
    p[0] = p1;
    p[1] = p2;
    p[2] = *p3;

    int n = 20;
    int len;

    //打开文件
    FILE* fpd = fopen(argv[1], "wb");
    if(fpd == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    // //读写文件
    // fwrite(&n, sizeof(n), 1, fpd);
    // fwrite(&p1, sizeof(p1), 1, fpd);

    fwrite(p, sizeof(struct person), 3, fpd);

    //关闭文件
    fclose(fpd);
    exit(0);
}