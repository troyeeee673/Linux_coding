//使用lockf文件锁防止读写混乱
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>


#define PROCNUM      20
#define BUFSIZE     1024

static int semid;

static void P()
{
    struct sembuf op;
    op.sem_op = -1;
    op.sem_flg = 0;
    op.sem_num = 0;
    while(semop(semid,&op, 1) < 0)
    {
        //真错
        if(errno != EINTR || errno != EAGAIN)
        {
            perror("semop()");
            exit(1);
        }
    }
}

static void V()
{
    struct sembuf op;
    op.sem_op = 1;
    op.sem_flg = 0;
    op.sem_num = 0;
    if(semop(semid, &op, 1) < 0)
    {
        perror("semop()");
        exit(1);
       
    }
}

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
   
    P();
    fgets(linebuf, BUFSIZE, fp);//读文件
    fseek(fp, 0, SEEK_SET);//定位到文件首
    fprintf(fp, "%d\n", atoi(linebuf)+1);//加一写回文件
    fflush(fp);
    lockf(fd, F_ULOCK, 0);
    fclose(fp);
    V();
    
   
}

int main()
{
    int i, j;
    pid_t pid;



    //创建sem，以PRIVATE方式(匿名)创建，只在亲缘关系进程间使用， 信号量数组元素数为1
    semid = semget(IPC_PRIVATE, 1, 0600);
    if(semid < 0)
    {
        perror("semget()");
        exit(1);
    }

    if(semctl(semid, 0, SETVAL, 1)< 0)//对id为semid的信号量的第0个下标的信号量进行SETVAL操作,设置其数量为1
    {
        perror("semctl()");
        exit(1);
    }


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

    semctl(semid, 0, IPC_RMID);
    exit(0);
}
