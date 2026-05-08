#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>

#define BUFFER_SIZE 256

// 读取线程函数
void* read_thread(void* arg) {
    int fd = *((int*)arg);
    char buffer[BUFFER_SIZE];
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read > 0) {
            printf("\r收到: %s\n> ", buffer);
            fflush(stdout);
        } else if (bytes_read == 0) {
            printf("\r对方已断开连接\n");
            break;
        } else {
            perror("read failed");
            break;
        }
    }
    return NULL;
}

int main() {
    const char* write_fifo = "/tmp/fifo_a_to_b";
    const char* read_fifo = "/tmp/fifo_b_to_a";
    
    // 创建两个管道
    if (mkfifo(write_fifo, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo write failed");
        return 1;
    }
    if (mkfifo(read_fifo, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo read failed");
        return 1;
    }
    
    printf("等待对方连接...\n");
    
    // 打开写端和读端
    int write_fd = open(write_fifo, O_WRONLY);
    int read_fd = open(read_fifo, O_RDONLY);
    
    if (write_fd == -1 || read_fd == -1) {
        perror("open failed");
        return 1;
    }
    
    printf("已连接！输入消息开始聊天（输入 'exit' 退出）\n");
    
    // 创建读取线程
    pthread_t thread;
    pthread_create(&thread, NULL, read_thread, &read_fd);
    
    // 主线程负责发送消息
    char buffer[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        
        write(write_fd, buffer, strlen(buffer));
    }
    
    close(write_fd);
    close(read_fd);
    unlink(write_fifo);
    unlink(read_fifo);
    
    return 0;
}