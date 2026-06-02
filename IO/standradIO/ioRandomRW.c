#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char** argv)
{
    char *buf;
    FILE*fp = fopen(argv[1], "r+");
    if(!fp)
    {
        perror("fopen()");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    printf("文件大小：%ld\n", size);
    buf = malloc(sizeof(char) * size);
    fseek(fp, 0, SEEK_SET);

    //移动文件指针到开头进行读取
    fread(buf, size, 1, fp);
    fprintf(stdout, "文件内容：%s\n", buf);
    fclose(fp);
    exit(0);
}