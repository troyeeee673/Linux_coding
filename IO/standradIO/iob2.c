#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFERSIZE  32

int main(int argc, char** argv)
{
    int buf[BUFFERSIZE];
    int len;
    FILE* fps = fopen(argv[1], "rb");
    if(!fps)
    {
        perror("fopen()");
        exit(1);
    }
    FILE* fpd = fopen(argv[2], "wb");
    if(!fpd)
    {
        fclose(fps);
        perror("fopen()");
        exit(1);
    }

    while(len = fread(buf, 1, BUFFERSIZE, fps))
    {
        fwrite(buf, 1, len, fpd);
    }

    fclose(fps);
    fclose(fpd);
    exit(0);
}