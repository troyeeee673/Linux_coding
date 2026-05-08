//使用lockf文件锁防止读写混乱
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define PROCNUM      20
#define BUFSIZE     1024

static void* func_add(void )
{
    int fd;
    char linebuf[BUFSIZE];
    FILE* fp;
    fp = fopen("./test.txt", "r+");
    if(fp == NULL)
    {
        perror("open()");
        exit(1);
    }
   
    fd = fileno(fp);
    lockf(fd, F_LOCK, 0); 

    fgets(linebuf, BUFSIZE, fp);//读文件
    fseek(fp, 0, SEEK_SET);//定位到文件首
    fprintf(fp, "%d\n", atoi(linebuf)+1);//加一写回文件
    fflush(fp);
    lockf(fd, F_ULOCK, 0);
    fclose(fp);
    
   
}

int main()
{
    int i, j;
    pid_t pid;
    
    for(i = 0; i < PROCNUM; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            for(j = 0 ; j < i ; j++)
            {
                wait(NULL);
            }
            perror("fork()");
            exit(1);
        }
        if(pid == 0 )
        {
            func_add();
            exit(0);
        }

    }
    for(j = 0; j < PROCNUM; j++)
        wait(NULL);
    exit(0);
}
