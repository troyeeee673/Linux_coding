#include "common.h"

#define MAX_CONNECTION  5


int main(int argc, char **argv)
{
    int sd;

    //1. 创建套接字
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        return EXIT_FAILURE;
    }

    //2. 端口复用
    int opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //3. 绑定地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERV_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len = sizeof(addr);
    bind(sd, (struct sockaddr*)&addr, len);

    //4. 监听
    listen(sd, MAX_CONNECTION);

    printf("服务端启动，监听端口 %d...\n", SERV_PORT);

    //5. 循环连接
    while(1)
    {
        struct sockaddr_in addr_client;
        socklen_t len_client = sizeof(addr_client);
        int sd_client = accept(sd, (struct sockaddr*)&addr_client, &len_client);
        if(sd_client < 0)
        {
            perror("accept()");
            continue;
        }
        printf("客户端接入: %s:%d\n", inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port));

        //1. 接受头部
        FileHeader header;
        recv(sd_client, &header, sizeof(header), 0);

        //2. 接收文件名
        char *filename = malloc(header.name_len + 8);
        recv(sd_client, filename, header.name_len, 0);
        filename[header.name_len] = '\0';

        printf("将要接收文件: %s，大小:%ld bytes\n", filename, header.file_size);

        //3. 创建文件用于接收数据
        strcat(filename, "_copy");
        int fd = open(filename, O_WRONLY | O_CREAT, 0644);
        if(fd < 0)
        {
            perror("open()");
            close(sd_client);
            continue;
        }

        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        if (!ctx) {
            perror("EVP_MD_CTX_new");
            close(fd);
            close(sd_client);
            continue;
        }
        EVP_DigestInit_ex(ctx, EVP_md5(), NULL);


        //4. 开始接收数据，一共header.file_size个字节
        char buf[MAX_BUFSIZE];
        long recv_total = 0;
        ssize_t n;
        while(recv_total < header.file_size)
        {
            // 1️⃣ 从 socket 接收数据
            size_t to_read = (header.file_size - recv_total) > MAX_BUFSIZE ? 
                            MAX_BUFSIZE : (header.file_size - recv_total);
            n = recv(sd_client, buf, to_read, 0);
            if(n <= 0) {
                if (n < 0) perror("recv()");
                break;
            }
            
            // 2️⃣ 写入磁盘文件
            ssize_t written = write(fd, buf, n);
            if (written != n) {
                perror("write()");
                break;
            }
            
            // 3️⃣ ✅ 更新 MD5 摘要（用接收到的数据）
            EVP_DigestUpdate(ctx, buf, n);  // 关键！每收到一块数据就更新 MD5
            
            recv_total += n;
            printf("\r已接收: %ld/%ld", recv_total, header.file_size);
            fflush(stdout);
        }
        printf("\n文件 %s 接收完成\n", filename);

        close(fd);
        close(sd_client);
    }
    close(sd);
    return EXIT_SUCCESS;
}


