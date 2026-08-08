#include "common.h"

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        perror("Usage!\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    //1. 创建套接字
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        return EXIT_FAILURE;
    }

    //2. 连接服务器
    struct sockaddr_in addr_serv;
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, (void *)&addr_serv.sin_addr.s_addr);

    if(connect(sd, (struct sockaddr*)&addr_serv, (socklen_t)sizeof(addr_serv))< 0)
    {
        perror("connect()");
        close(sd);
        return EXIT_FAILURE;
    }

    //3. 打开待上传的文件
    int fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        perror("open()");
        close(sd);
        return EXIT_FAILURE;
    }

    //4. 获取文件大小、文件名大小
    struct stat s;
    fstat(fd, &s);
    ssize_t file_size = s.st_size;
    char file_name[256];  // 定义在栈上，有实际内存
    char *p = strrchr(filename, '/');
    if (p)
        strcpy(file_name, p + 1);
    else
        strcpy(file_name, filename);
    int name_len = strlen(file_name);


    unsigned char md5[MD5_LEN];
    if (compute_file_md5(filename, md5) < 0) {
        fprintf(stderr, "计算 MD5 失败\n");
        close(fd);
        close(sd);
        return EXIT_FAILURE;
    }
    
    printf("文件 MD5: ");
    print_md5(md5);

    //5. 发送文件头信息
    FileHeader header = {
        .file_size = file_size,
        .name_len = name_len,
        .md = md5,
    };

    send(sd, &header, sizeof(header), 0);
    send(sd, file_name, name_len, 0);
    printf("准备上传文件: %s, size: %ld bytes\n", file_name, file_size);

    //6. sendfile 零拷贝发送文件
    off_t  offset  = 0;
    ssize_t send_len;
    ssize_t sent = 0;
    while(sent < file_size)
    {
        send_len = sendfile(sd, fd, &offset, MAX_BUFSIZE);
        if(send_len <= 0)
        {
            perror("snedfile()");
            continue;
        }
        sent += send_len;
        printf("\r已发送: %ld/%ld", sent, file_size);

    }
    printf("\n文件发送完毕\n");
    close(fd);
    close(sd);
    return EXIT_SUCCESS;
    
}

