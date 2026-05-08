//编写代码，实现有亲缘关系的进程间管道机制通信

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFSIZE 1024

int main()
{
    int pd[2], fd;
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
       close(pd[1]);//关闭写端
       dup2(pd[0], 0);//输入重定向到pd[0]
       close(pd[0]);//关闭原来的读端，采用从标准输入读取
       fd = open("/dev/null", O_RDWR);
       dup2(fd, 1);
       dup2(fd, 2);
       execl("/usr/bin/mpg123", "mpg123", "-", NULL);//执行解码器
       perror("execl()");
       exit(1);
    }

    //父进程写
    else
    {
        close(pd[0]);
        //从网络收取数据，写入管道
        close(pd[1]);
        wait(NULL);
        exit(0);
    }

    
}