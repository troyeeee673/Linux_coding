//编写代码，实现有亲缘关系的进程间管道机制通信

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 1024

int main()
{
    int pd[2];
    pid_t pid;
    int len;
    char buf[BUFSIZE];

    if(pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    //子进程读
    if(pid == 0)
    {
        close(pd[1]);
        len = read(pd[0], buf, BUFSIZE);
        write(1, buf, len);
        close(pd[0]);
        exit(0);
    }

    //父进程写
    else
    {
        close(pd[0]);
        write(pd[1], "Hello World\n", 12);
        close(pd[1]);
        wait(NULL);
        exit(0);
    }

    
}