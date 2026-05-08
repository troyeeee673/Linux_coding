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

int main(int argc, char **argv)
{
    int sd;
    struct msg_st sbuf;
    struct sockaddr_in6 laddr, raddr;
    
    sd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 发送端：设置组播跳数（可选，默认1）
    int hops = 5;  // TTL
    if(setsockopt(sd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops)) < 0)
    {
        perror("setsockopt() IPV6_MULTICAST_HOPS");
        exit(1);
    }

    // 如果需要指定出口接口（可选）
    int ifindex = if_nametoindex("eth0");  // 或者用 0 让系统选择
    if(setsockopt(sd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex, sizeof(ifindex)) < 0)
    {
        perror("setsockopt() IPV6_MULTICAST_IF");
        exit(1);
    }

    // 绑定发送端口
    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(atoi(SNDPORT));
    laddr.sin6_addr = in6addr_any;
    laddr.sin6_scope_id = 0;
    
    if(bind(sd, (struct sockaddr*)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    //先清空sbuf中的数据，防止发包时造成数据泄露
    memset(&sbuf, '\0', sizeof(sbuf));
    strcpy(sbuf.name, "Cassid");
    sbuf.math = htonl(100);
    sbuf.chinese = htonl(99);

    // 设置目标地址（组播组）
    raddr.sin6_family = AF_INET6;
    raddr.sin6_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET6, MTGROUP, &raddr.sin6_addr);
    raddr.sin6_scope_id = if_nametoindex("eth0");  // 必须指定接口
    raddr.sin6_flowinfo = 0;

    if(sendto(sd, &sbuf, sizeof(sbuf), 0, (struct sockaddr*)&raddr, sizeof(raddr)) < 0)
    {
        perror("sendto()");
        exit(1);
    }
    puts("OK");

    close(sd);
    exit(0);
}