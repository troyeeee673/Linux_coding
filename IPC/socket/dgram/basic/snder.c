#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>      
#include <arpa/inet.h>
#include <sys/types.h>
#include <net/if.h>

#include "proto.h"

int main(int argc ,char ** argv)
{
    int sd;
    struct msg_st sbuf;
    struct sockaddr_in6 laddr, raddr;
    if(argc < 2)
    {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }
    sd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }
    //可省略,随机分配发送端口
    //如要固定端口需要显式bind
    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(atoi(SNDPORT));
    laddr.sin6_addr = in6addr_any;
    laddr.sin6_scope_id = if_nametoindex("eth0"); //scope_id 告诉系统使用哪个接口
    laddr.sin6_flowinfo = 0;//用于 IPv6 的流标签（Flow Label）和流量类别（Traffic Class）
    
    bind(sd, (struct sockaddr*)&laddr, sizeof(laddr));

    strcpy(sbuf.name, "Vivi");
    sbuf.math = htonl(100);
    sbuf.chinese = htonl(99);

    raddr.sin6_family = AF_INET6;
    raddr.sin6_port = htons(atoi(RCVPORT));
    //将argv[1]的IP地址转换为大整数，作为远端ip
    inet_pton(AF_INET6,argv[1], &raddr.sin6_addr);
    

    if(sendto(sd, &sbuf, sizeof(sbuf), 0, (struct sockaddr*)&raddr, sizeof(raddr))< 0)
    {
        perror("sendto()");
        exit(1);
    }
    puts("OK");



    close(sd);
    exit(0);
}