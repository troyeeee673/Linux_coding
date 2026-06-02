#include <stdio.h>
#include<ctype.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    char ch;
    if(argc < 3)
    {
        fprintf(stderr, "Usage:...");
        exit(1);
    }
    FILE* fps, *fpd;
    fps = fopen(argv[1], "r+");
    if(fps == NULL)
    {
        fprintf(stderr, "打开文件%s失败", argv[1]);
        exit(1);
    }
    fpd = fopen(argv[2], "w+");
    if(fpd == NULL)
    {
        fclose(fps);
        fprintf(stderr, "打开文件%s失败", argv[2]);
        exit(1);
    }
    while((ch = fgetc(fps)) != EOF)
    {
        //简单加密
        // if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <='Z')
        // {
        //     ch ++;
        // }
        //isalpha(ch);
        fputc(ch, fpd);
    }
    fclose(fps);
    fclose(fpd);
    exit(0);

}