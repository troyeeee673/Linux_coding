#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#include <sys/un.h> //用于本地通信
#include <netinet/in.h>

int main()
{
    // 1. 创建服务端套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 2. 绑定地址
    //  sturct sockaddr_un addr;用于本地通信
    struct sockaddr_in addr;        // 用于网络通信
    memset(&addr, 0, sizeof(addr)); // 初始化addr空间为0
    addr.sin_family = AF_INET;
    // 端口号，从1024往后使用
    addr.sin_port = htons(9000);
    // 地址本质是一个无符号32位整数
    // 这里的地址写INADDR_ANY，相当于0.0.0.0,表示服务器可以接收任何ip的请求
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    // 3. 监听
    listen(fd, 9); // 9是最大请求个数
    printf("服务器准备接受请求\n");

    // 4. 接收请求（接收多个）
    int falg = 1;
    int client_active = 1;
    while (falg)
    {
        // 用于接收client数据的
        struct sockaddr_in addr_client;
        memset(&addr_client, 0, sizeof(addr_client));
        // 接受请求
        printf("服务器等待连接\n");
        // 建立一个连接
        //  必须单独定义一个socklen_t变量存长度
        socklen_t addr_len = sizeof(addr_client);
        int fd_client = accept(fd, (struct sockaddr *)&addr_client, &addr_len);

        char buf[1024]; // 存数据
        memset(buf, 0, sizeof(buf));

        while (client_active)
        {
            // 读写数据（建立一个子线程/进程进行处理）
            int size = read(fd_client, buf, sizeof(buf));
            if (size <= 0) 
            {
                printf("客户端断开连接\n");
                break;
            }
            printf("收到：%s\n", buf);
            if (strcmp(buf, "exit\n") == 0)
            {
                client_active = 0;
                printf("客户端取消链接\n");
                break;
                // close(fd_client);
            }
            else
            {
                char message[] = "This is a message from server.\n";
                write(fd_client, message, sizeof(message));
            }
        }

        // 5 .关闭客户端连接
        close(fd_client);
    }
    close(fd);
    return 0;
}