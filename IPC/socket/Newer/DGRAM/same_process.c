#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

static int sd;

void *my_sender(void *arg)
{
    char msg[1024];
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1900);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    socklen_t len = sizeof(addr);

    while (1)
    {
        memset(msg, 0, sizeof(msg));
        printf(":");
        fgets(msg, 1024, stdin);

        // 1. 发送数据
        ssize_t size = sendto(sd, msg, sizeof(msg), 0, (struct sockaddr *)&addr, len);
        if (size <= 0)
        {
            perror("sendto()");
            break;
        }
        if(strncmp(msg, "bye", 3) == 0)
        {
            printf("About to end this communication\n");
            break;
        }

        char *p = strchr(msg, '\n');
        if (p != NULL)
        {
            *p = '\0';
        }
        printf("send data:%s   length = %ld\n", msg, size);
    }
    return NULL;
}

void *my_recver(void *arg)
{
    char buf[1024];
    struct sockaddr_in addr_send, addr_recv;

    int opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t)sizeof(opt));

    socklen_t len = sizeof(addr_recv);
    addr_recv.sin_family = AF_INET;
    addr_recv.sin_port = htons(1900);
    addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定地址
    bind(sd, (struct sockaddr *)&addr_recv, len);

    // 接收数据
    while (1)
    {
        memset(&addr_send, 0, sizeof(addr_send));
        memset(&addr_recv, 0, sizeof(addr_recv));
        len = sizeof(addr_send);
        ssize_t size = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&addr_send, &len);
        if (size <= 0)
        {
            perror("recvfrom()");
            close(sd);
            exit(1);
        }
        char *p = strchr(buf, '\n');
        if (p)
        {
            *p = '\0';
        }
        if(strncmp(buf, "bye", 3) == 0)
        {
            close(sd);
            printf("Communication end\n");
            break;
        }
        printf("recv data :%s  lenght = %ld\n", buf, size);
    }
    return NULL;
}

int main()
{
    pthread_t tids[2];
    int i = 0;
    // 1. 创建套接字文件
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 2. 创建线程，分别用于发送、接收
    pthread_create(&tids[0], NULL, my_recver, NULL);
    pthread_create(&tids[0], NULL, my_sender, NULL);

    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);

    close(sd);
    return 0;
}