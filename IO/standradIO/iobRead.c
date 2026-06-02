#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "iob.h"

int main(int argc, char** argv)
{
    //int* p1 = malloc(sizeof(int));
    struct person p2[3];
    FILE* fp = fopen(argv[1], "rb");
    if(!fp)
    {
        perror("fopen()");
        exit(1);
    }

    // fread(p1, sizeof(int), 1, fp);
    fread(p2, sizeof(struct person), 3, fp);


    //fprintf(stdout, "n = %d\n", *p1);
    for(int i = 0 ;i < 3;i++)
    {
        fprintf(stdout, "p2[%d]->name = %s, p2[%d]->age = %d\n", i, p2[i].name, i, p2[i].age);
    }

    fclose(fp);
    exit(0);
}