#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>      
#include <arpa/inet.h>
#include <sys/types.h>

#include "proto.h"

#define IPSTRSIZE   40

int main()
{
    int sd;
    struct sockaddr_in6 laddr, raddr;
    struct msg_st recv_buf;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];

    sd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET6, "::", &laddr.sin6_addr);  // 修正：IPv6 通配地址

    if(bind(sd, (const struct sockaddr*)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    raddr_len = sizeof(raddr);
    while(1)
    {
        // 修正：raddr_len 取地址，raddr 去掉 const
        recvfrom(sd, &recv_buf, sizeof(recv_buf), 0, 
                 (struct sockaddr*)&raddr, &raddr_len);
        
        inet_ntop(AF_INET6, &raddr.sin6_addr, ipstr, IPSTRSIZE);
        printf("---MESSAGE FROM %s : %d---\n", ipstr, ntohs(raddr.sin6_port));
        printf("NAME = %s\n", recv_buf.name);
        printf("MATH = %d\n", ntohl(recv_buf.math));
        printf("CHINESE = %d\n", ntohl(recv_buf.chinese));
        printf("\n");
    }

    close(sd);  // 修正：添加 sd 参数

    exit(0);
}