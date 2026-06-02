#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE* fp = fopen("mycpy.c", "r+");

    if(fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    char buf[128];
    char* res;
    int count = 0;
    while(res = fgets(buf, 127, fp) != NULL)
    {
        printf("%s", buf);
        printf("%s", res);
        count++;
    }

    fclose(fp);
    exit(0);
}