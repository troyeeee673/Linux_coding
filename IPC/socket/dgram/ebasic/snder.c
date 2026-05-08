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

// ./snder IP NAME

int main(int argc ,char ** argv)
{
    int sd;
    struct msg_st *sbufp;
    struct sockaddr_in6 laddr, raddr;
    int size;
    int namesize;
    if(argc < 3)
    {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }
    if((namesize = strlen(argv[2])) > NAMEMAX)
    {
        fprintf(stderr, "Name is too long");
        exit(1);
    }

    sd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    size = sizeof(struct msg_st) + namesize;//申请空间，存放发送数据
    sbufp = malloc(size);
    if(sbufp == NULL)
    {
        perror("malloc()");
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

   
    strcpy(sbufp->name, argv[2]);
    sbufp->math = htonl(100);
    sbufp->chinese = htonl(99);

    raddr.sin6_family = AF_INET6;
    raddr.sin6_port = htons(atoi(RCVPORT));
    //将argv[1]的IP地址转换为大整数，作为远端ip
    inet_pton(AF_INET6,argv[1], &raddr.sin6_addr);
    

    if(sendto(sd, sbufp, size, 0, (struct sockaddr*)&raddr, sizeof(raddr))< 0)
    {
        perror("sendto()");
        exit(1);
    }
    puts("OK");



    close(sd);
    exit(0);
}