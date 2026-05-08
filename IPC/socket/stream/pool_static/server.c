#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>      
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

#include "proto.h"

#define IPSTRSIZE       128
#define BUFSIZE         1024
#define PROCNUM         4

static void server_job(int sd)
{
    char buf[BUFSIZE];
    int len;
    len = sprintf(buf, FMT_STAMP, (long long)time(NULL));
    if(send(sd, buf, len, 0) < 0)
    {
        perror("send()");
        exit(1);
    }
}
static void server_loop(int);

int main()
{
    int sd;
    pid_t pid;
    struct sockaddr_in6 laddr;
        int i;
    //用AF_INET6域中的SOCK_STREAM的形式以默认协议进行传输（这里默认是IPPRPOTO_TCP）
    sd = socket(AF_INET6, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    //解决"Address already in use"错误
    int val = 1;  // 1 表示启用，0 表示禁用
if(setsockopt(sd,                    // socket 文件描述符
              SOL_SOCKET,            // 选项层级：通用 socket 层
              SO_REUSEADDR,          // 选项名称：地址重用
              &val,                  // 选项值（指向整数的指针）
              sizeof(val)) < 0)      // 选项值的大小
{
    perror("setsockopt()");
    exit(1);
}
    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET6,"::",&laddr.sin6_addr);


    if(bind(sd, (struct sockaddr *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    //200是全连接数
    if(listen(sd, 200) < 0)
    {
        perror("listen()");
        exit(1);
    }

    for(i = 0 ;i < PROCNUM ;i ++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if(pid == 0)
        {
            server_loop(sd);
            exit(0);
        }
    }
     
    for(i = 0 ;i < PROCNUM ;i++)
    {
        wait(NULL);
    }

    close(sd);
    exit(0);

}

static void server_loop(int sd)
{
    struct sockaddr_in6 raddr;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];
    int sd2;

    raddr_len = sizeof(raddr);

    while(1)
    {
        if((sd2 = accept(sd, (void *)&raddr, &raddr_len)) < 0)
        {
            perror("accept()");
            exit(1);
        }
        inet_ntop(AF_INET6, &raddr.sin6_addr, ipstr, IPSTRSIZE);
        printf("[%d]Client:%s:%d\n", getpid(),ipstr, ntohs(raddr.sin6_port));
        
        server_job(sd2);
        close(sd2);
       
    }
}

