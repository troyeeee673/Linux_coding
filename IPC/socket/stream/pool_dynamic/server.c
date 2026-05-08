#define _GNU_SOURCE
#include <stdio.h>       // printf, perror 等
#include <stdlib.h>      // exit, malloc 等
#include <sys/types.h>   // pid_t 等基本类型
#include <sys/socket.h>  // socket, setsockopt, bind, listen, accept
#include <netinet/in.h>  // sockaddr_in, htons, INADDR_ANY
#include <unistd.h>      // close, fork, getpid
#include <arpa/inet.h>   // inet_ntop, inet_pton (打印IP地址时常用)
#include <fcntl.h>       // fcntl (设置非阻塞等)
#include <time.h>        // time (随机数种子等)
#include <sys/wait.h>    // waitpid, WNOHANG
#include <signal.h>      // sigaction, sigemptyset, SIGUSR1 等
#include <string.h>      // memset, strerror (错误信息)
#include <errno.h>       // errno (错误码)
#include <sys/mman.h>

#include "proto.h"
#define MINSPARESERVER  5   //最小空闲服务器个数
#define MAXSPARESERVER  10  //最大空闲服务器个数
#define MAXCLIENT       20  //最大客户端访问数
#define LINEBUFSIZE     64
#define IPSTRSIZE       64
//定义信号
#define SIG_NOTIFY      SIGUSR2

enum
{
    STATE_IDLE = 0,
    STATE_BUSY
};

struct server_st
{
    pid_t pid;
    int state;
    //int reuse;//记录每个进程服务次数，达到一定次数消亡该进程
};

static struct server_st* serverpool;
static int idle_count = 0, busy_count = 0;
static int sd;


static void usr2_handler(int s)
{
    return;
}

static int scan_pool()
{
    int i, idle = 0, busy = 0;
    for(i = 0 ;i < MAXCLIENT;i++)
    {
        if(serverpool[i].pid == -1)
            continue;
        
        //检查一个进程是否存在
        if(kill(serverpool[i].pid, 0) == -1)
        {
            serverpool[i].pid = -1;
            continue;
        }

        if(serverpool[i].state == STATE_IDLE)
            idle++;
        else if(serverpool[i].state == STATE_BUSY)
            busy++;
            else
            {
                fprintf(stderr, "Unknow state!\n");
               // _exit(1);//直接退出
                abort();
            }
    }
    idle_count = idle;
    busy_count = busy;
    return 0;
}

static void server_job(int pos)
{
    pid_t ppid;
    struct sockaddr_in6 raddr;
    socklen_t raddr_len = sizeof(raddr);
    int client_sd;
    time_t stamp;
    char linebuf[LINEBUFSIZE];  // 改为数组，不需要动态分配
    char ipstr[IPSTRSIZE];   
    int len;
    
    ppid = getppid();
    while(1)
    {
        serverpool[pos].state = STATE_IDLE;
        kill(ppid, SIG_NOTIFY);
        raddr_len = sizeof(raddr);  // 每次循环都要重置
        client_sd = accept(sd, (void *)&raddr, &raddr_len);
        if(client_sd < 0)
        {
            if(errno != EINTR && errno!= EAGAIN)
            {
                perror("accept()");
                exit(1);
            }
        }

        serverpool[pos].state = STATE_BUSY;
        kill(ppid, SIG_NOTIFY);
        inet_ntop(AF_INET6, &raddr.sin6_addr, ipstr, IPSTRSIZE);
        //printf("[%d]client : %s :%d \n", getpid(), ipstr, ntoh(raddr.sin6_port));

        stamp = time(NULL);
        len = snprintf(linebuf, LINEBUFSIZE, FMT_STAMP, stamp);
        send(client_sd, linebuf, len, 0);
        sleep(3);
        close(client_sd);
    }
}

static int add_1_server()
{
    int slot;
    pid_t pid;
    if(idle_count + busy_count >= MAXCLIENT)
    {
        return -1;
    }
    //找空位
    for(slot = 0 ;slot < MAXCLIENT;slot++)
    {
        if(serverpool[slot].pid == -1)
        {
            break;
        }
    }
    serverpool[slot].state = STATE_IDLE;
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)
    {
        server_job(slot);
        exit(0);
    }
    else
    {
        serverpool[slot].pid = pid;
        idle_count++;
    }
    return 0;
}

static int del_1_server()
{
    int i ;
    if(idle_count == 0)
        return -1;
    for(i = 0 ;i < MAXCLIENT;i++)
    {
        if(serverpool[i].pid != -1 && serverpool[i].state == STATE_IDLE)
        {
            kill(serverpool[i].pid, SIGTERM);
            serverpool[i].pid = -1;
            idle_count --;
            break;
        }
    }
    return 0;
}

int main()
{

    int i;
    int val = 1;
    struct sigaction sa, oldsa;
    sigset_t set, oldset;
    struct sockaddr_in6 laddr, raddr;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = SA_NOCLDWAIT;//阻止子进程成为僵尸进程
    sigaction(SIGCHLD, &sa, &oldsa);

    sa.sa_handler = usr2_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &sa, &oldsa);

    sigemptyset(&set);
    sigaddset(&set, SIG_NOTIFY);
    sigprocmask(SIG_BLOCK, &set, &oldset);

    //分配内存并初始化
    //设置了MAP_ANONYMOUS会将空间初始化为0，并且设置的fd无效
    serverpool = mmap(NULL, sizeof(struct server_st) * MAXCLIENT, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(serverpool == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    //初始化
    for(i = 0 ;i < MAXCLIENT ;i ++)
    {
        serverpool[i].pid = -1;
    }

    sd = socket(AF_INET6, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }


    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET6, "::", (&laddr.sin6_addr));

    if(bind(sd, (struct sockaddr *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    if(listen(sd, 100) < 0)
    {
        perror("listen()");
        exit(1);
    }

    for(i = 0 ;i < MINSPARESERVER; i++)
    {
        add_1_server();
    }

    while(1)
    {
        sigsuspend(&oldset);
        scan_pool();//遍历池的状态

        //contrl the pool
        //空闲个数大于最大空闲个数，将多余的杀掉
        if(idle_count > MAXSPARESERVER)
        {
            for(i = 0 ; i < (idle_count - MAXSPARESERVER) ;i++)
            {
                del_1_server();
            }
        }
        else if(idle_count < MINSPARESERVER)
        {
            for(i = 0 ;i < (MINSPARESERVER - idle_count); i++)
            {
                add_1_server();
            }
        }

        //输出当前池的状态
        for(i = 0 ;i < MAXCLIENT;i++)
        {
            if(serverpool[i].pid == -1)
                putchar(' ');
            else if(serverpool[i].state == STATE_IDLE)
                putchar('.');
            else
                putchar('x');
            
        }
        putchar('\n');
    }

    sigprocmask(SIG_SETMASK, &oldset, NULL);//恢复信号集


    exit(0);
}