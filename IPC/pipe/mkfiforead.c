#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 256

int main() {
    const char* fifo_path = "/tmp/my_fifo";
    
    // 创建命名管道（如果不存在）
    if (mkfifo(fifo_path, 0666) == -1) {
        if (errno != EEXIST) {  // 如果已存在，不算错误
            perror("mkfifo failed");
            return 1;
        }
    }
    
    printf("等待写入端连接...\n");
    
    // 打开 FIFO 进行读取（会阻塞直到有写入端打开）
    int fd = open(fifo_path, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    
    printf("写入端已连接，开始接收消息：\n");
    
    char buffer[BUFFER_SIZE];
    while (1) {
        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);
        
        // 读取消息
        ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read > 0) {
            printf("收到: %s\n", buffer);
        } else if (bytes_read == 0) {
            printf("写入端已关闭连接\n");
            break;
        } else {
            perror("read failed");
            break;
        }
    }
    
    close(fd);
    // 删除 FIFO 文件
    unlink(fifo_path);
    printf("读取端关闭\n");
    return 0;
}