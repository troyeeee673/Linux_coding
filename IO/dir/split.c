#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 64

int main(int argc, char **argv)
{
    char buf[BUFSIZE];
    int len;
    char split[32];
    int i = 1;
    FILE *fp = fopen(argv[1], "r+");
    if (!fp)
    {
        perror("fopen()");
        exit(1);
    }

    while ((len = fread(buf, 1, BUFSIZE, fp)))
    {
        sprintf(split, "split_%d.txt", i);
        FILE *fpd = fopen(split, "w+");
        if (!fpd)
        {
            fclose(fp);
            perror("fopen()");
            exit(1);
        }
        fwrite(buf, 1, len, fpd);
        i++;
    }
}