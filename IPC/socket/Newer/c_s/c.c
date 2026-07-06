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
    int sd1;
    char buf[1024];
    FILE *fp;
    
    // 1. 创建套接字
    if ((sd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 定义要连接的服务器地址
    struct sockaddr_in addr_s;
    memset(&addr_s, 0, sizeof(addr_s));
    addr_s.sin_family = AF_INET;
    addr_s.sin_port = htons(1900);
    inet_pton(AF_INET, "127.0.0.1", &addr_s.sin_addr);

    // 2. 建立连接
    if (connect(sd1, (struct sockaddr *)&addr_s, sizeof(addr_s)) < 0)
    {
        perror("connect");
        exit(1);
    }
    
    fp = fdopen(sd1, "r+");
    if (fp == NULL)
    {
        perror("fdopen()");
        close(sd1);
        exit(1);
    }
    
    // 设置缓冲区为行缓冲模式
    setvbuf(fp, NULL, _IOLBF, 0);
    
    // 3. 读写
    while (1)
    {
        memset(buf, 0, sizeof(buf));

        // 发送数据
        printf("input here: ");
        fgets(buf, sizeof(buf), stdin);
        fprintf(fp, "%s", buf);
        fflush(fp);
        
        if (strncmp(buf, "bye", 3) == 0)
        {
            // 读取服务器的 bye 回复
            memset(buf, 0, sizeof(buf));
            if (fgets(buf, sizeof(buf), fp) != NULL)
            {
                printf("服务器回复: %s", buf);
            }
            printf("客户端退出\n");
            break;
        }

        // 读取服务器回复（确保服务器会发送回复）
        memset(buf, 0, sizeof(buf));
        if (fgets(buf, sizeof(buf), fp) != NULL)
        {
            printf("服务器回复: %s", buf);
        }
        else
        {
            printf("服务器断开连接\n");
            break;
        }
    }

    // 4. 关闭连接
    fclose(fp);
    return 0;
}