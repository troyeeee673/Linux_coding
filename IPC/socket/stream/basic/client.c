#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>      
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

#include "proto.h"

int main(int argc, char ** argv)
{
    int sd;
    FILE* fp;
    struct sockaddr_in6 raddr;
    long long stamp;
    if(argc < 2)
    {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    sd = socket(AF_INET6, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // bind();

    raddr.sin6_family = AF_INET6;
    raddr.sin6_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET6, argv[1], &raddr.sin6_addr);

    if(connect(sd, (void *)&raddr, sizeof(raddr)) < 0)
    {
        perror("connect()");
        exit(1);
    }

    //将文件描述符sd,加上权限，转变成流FILE*的类型fp,这就成为了可移植的了
    fp = fdopen(sd, "r+");
    if(fp == NULL)
    {
        perror("fdopen()");
        exit(1);
    }

    if(fscanf(fp, FMT_STAMP, &stamp) < 1)
    {
        fprintf(stderr, "Bad format\n");
    }
    else
    {
        fprintf(stdout , "stamp = %lld\n", stamp);
    }
    fclose(fp);
    //rcve();
    //close();

    exit(0);
}