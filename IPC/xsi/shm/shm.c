#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MEMSIZE     1024
int main()
{
    pid_t pid;
    int shmid;
    void *ptr;
    //ftok();

    shmid = shmget(IPC_PRIVATE, MEMSIZE, 0600);//shm的创建
    if(shmid < 0)
    {
        perror("shmget()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)
    {
        //系统自动找一块可分配的内存，存放编号为shmid的共享内存块， 没有特殊要求（0）
        ptr = shmat(shmid, NULL, 0);
        if(ptr == (void *)-1)
        {
            perror("shmat()");
            exit(1);
        }
        strcpy(ptr, "Hello");
        shmdt(ptr);//解除映射
        exit(0);
    }
    else
    {
        wait(NULL);
        ptr = shmat(shmid, NULL, 0);
        if(ptr == (void *)-1)
        {
            perror("shmat()");
            exit(1);
        }
        puts(ptr);
        shmdt(ptr);
        shmctl(shmid, IPC_RMID, NULL);//shm的实例消除,不需要参数（第三个参数为NULL）

         exit(0);
    }
    


}