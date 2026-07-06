#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    int sd;
    FILE *fp;

    // 1. 创建套接字
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 设置端口复用
    int opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. 绑定地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(1900);

    if (bind(sd, (struct sockaddr *)&addr, (socklen_t)sizeof(addr)) < 0)
    {
        perror("bind()");
        close(sd);
        exit(1);
    }

    // 3. 监听
    if (listen(sd, 10) < 0)
    {
        perror("listen()");
        close(sd);
        exit(1);
    }

    printf("服务器启动，等待连接...\n");

    char buf[1024];
    struct sockaddr_in addr_client;
    memset(&addr_client, 0, sizeof(addr_client));
    socklen_t len = sizeof(addr_client);

    while (1)
    {

        // 4. 接受连接
        int sd_client = accept(sd, (struct sockaddr *)&addr_client, &len);
        if (sd_client < 0)
        {
            perror("accept()");
            close(sd);
            exit(1);
        }

        int port = ntohs(addr_client.sin_port);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_client.sin_addr, ip, INET_ADDRSTRLEN);
        printf("客户端已连接：%s:%d\n", ip, port);

        fp = fdopen(sd_client, "r+");
        if (fp == NULL)
        {
            perror("fdopen()");
            close(sd_client);
            close(sd);
            exit(1);
        }

        // 设置行缓冲
        setvbuf(fp, NULL, _IOLBF, 0);
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork()");
            close(sd);
            exit(1);
        }
        if (pid == 0)
        {
            while (1)
            {
                // 5. 读取客户端数据
                memset(buf, 0, sizeof(buf));
                if (fgets(buf, sizeof(buf), fp) == NULL)
                {
                    printf("客户端断开连接\n");
                    break;
                }

                printf("收到客户端消息: %s", buf);

                // 检查是否是退出命令
                if (strncmp(buf, "bye", 3) == 0)
                {
                    printf("收到退出命令，发送bye回复\n");
                    fprintf(fp, "bye\n");
                    fflush(fp);
                    break;
                }

                // 发送回复
                char msg[] = "ok";
                fprintf(fp, "server send data: %s\n", msg);
                fflush(fp);
            }
        }

        // 6. 清理资源
        fclose(fp);
        close(sd_client);
    }

    close(sd);
    printf("服务器关闭\n");
    return 0;
}