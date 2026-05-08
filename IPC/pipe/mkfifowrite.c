#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main() {
    const char* fifo_path = "/tmp/my_fifo";
    
    // 创建命名管道（如果不存在）
    if (mkfifo(fifo_path, 0666) == -1) {
        if (errno != EEXIST) {  // 如果已存在，不算错误
            perror("mkfifo failed");
            return 1;
        }
    }
    
    printf("等待读取端打开...\n");
    
    // 打开 FIFO 进行写入（会阻塞直到有读取端打开）
    int fd = open(fifo_path, O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    
    printf("读取端已连接，开始发送消息（输入 'exit' 退出）\n");
    
    char buffer[256];
    while (1) {
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
        
        // 去除换行符
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        
        // 发送消息（包含字符串结束符）
        ssize_t bytes_written = write(fd, buffer, strlen(buffer));
        if (bytes_written == -1) {
            perror("write failed");
            break;
        }
    }
    
    close(fd);
    printf("写入端关闭\n");
    return 0;
}