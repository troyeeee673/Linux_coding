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

    // IPv6 组播：加入组播组
    struct ipv6_mreq mreq;
    inet_pton(AF_INET6, MTGROUP, &mreq.ipv6mr_multiaddr);
    mreq.ipv6mr_interface = 0;  // 0 表示让系统选择默认接口，或用 if_nametoindex("eth0")
    
    // 关键修正：使用 IPPROTO_IPV6 和 IPV6_JOIN_GROUP
    if(setsockopt(sd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt() IPV6_JOIN_GROUP");
        exit(1);
    }

    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET6, "::", &laddr.sin6_addr);

    if(bind(sd, (const struct sockaddr*)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    raddr_len = sizeof(raddr);
    while(1)
    {
        recvfrom(sd, &recv_buf, sizeof(recv_buf), 0, 
                 (struct sockaddr*)&raddr, &raddr_len);
        
        inet_ntop(AF_INET6, &raddr.sin6_addr, ipstr, IPSTRSIZE);
        printf("---MESSAGE FROM %s : %d---\n", ipstr, ntohs(raddr.sin6_port));
        printf("NAME = %s\n", recv_buf.name);
        printf("MATH = %d\n", ntohl(recv_buf.math));
        printf("CHINESE = %d\n", ntohl(recv_buf.chinese));
        printf("\n");
    }

    close(sd);
    exit(0);
}