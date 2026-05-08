// 痛过mmap内存映射，实现父子进程通信（子进程复制父进程资源，包括内存映射）

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>  // 定义 pid_t 类型
#include <sys/wait.h>   // 定义 wait() 函数
#include <string.h> 


#define MEMSIZE 1024

int main(int argc, char** argv)
{
    char* ptr;
    pid_t pid;
    //申请一块大小为memsize的内存空间，并自动初始化为0
    ptr = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        munmap(ptr, MEMSIZE);//父进程中已经内存映射成功
        exit(1);
    }

    if(pid == 0)//child_write
    {
        strcpy(ptr, "hello");
        munmap(ptr, MEMSIZE);//子进程自己的内存映射
        exit(0);
    }
    else
    {
        wait(NULL);
        puts(ptr);
        munmap(ptr, MEMSIZE);
    }

    exit(0);
}