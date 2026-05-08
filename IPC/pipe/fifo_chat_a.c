#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>

#define BUFFER_SIZE 256

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// 读取线程
void* read_thread(void* arg) {
    int read_fd = *((int*)arg);
    char buffer[BUFFER_SIZE];
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(read_fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read > 0) {
            pthread_mutex_lock(&print_mutex);
            printf("\n\033[33m对方: %s\033[0m\n", buffer);
            printf("你: ");
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex);
        } else if (bytes_read <= 0) {
            pthread_mutex_lock(&print_mutex);
            printf("\n对方已断开连接\n");
            pthread_mutex_unlock(&print_mutex);
            break;
        }
    }
    return NULL;
}

int main() {
    const char* fifo_a_to_b = "/tmp/fifo_a_to_b";
    const char* fifo_b_to_a = "/tmp/fifo_b_to_a";
    
    printf("\033[34m聊天程序A（等待B连接...）\033[0m\n");
    
    // 清理可能存在的旧管道
    unlink(fifo_a_to_b);
    unlink(fifo_b_to_a);
    
    // 创建两个管道
    if (mkfifo(fifo_a_to_b, 0666) == -1 && errno != EEXIST) {
        perror("创建管道失败");
        return 1;
    }
    if (mkfifo(fifo_b_to_a, 0666) == -1 && errno != EEXIST) {
        perror("创建管道失败");
        return 1;
    }
    
    // A打开：写给B的管道（写端）
    printf("等待B连接...\n");
    int write_fd = open(fifo_a_to_b, O_WRONLY);
    if (write_fd == -1) {
        perror("打开写管道失败");
        return 1;
    }
    
    // 从B接收的管道（读端）
    int read_fd = open(fifo_b_to_a, O_RDONLY);
    if (read_fd == -1) {
        perror("打开读管道失败");
        close(write_fd);
        return 1;
    }
    
    printf("\033[32m已连接！开始聊天（输入 'exit' 退出）\033[0m\n");
    
    // 创建读取线程
    pthread_t thread;
    pthread_create(&thread, NULL, read_thread, &read_fd);
    pthread_detach(thread);
    
    // 主线程发送消息
    char buffer[BUFFER_SIZE];
    printf("你: ");
    fflush(stdout);
    
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strlen(buffer) == 0) continue;  // 忽略空行
        
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        
        pthread_mutex_lock(&print_mutex);
        ssize_t bytes_written = write(write_fd, buffer, strlen(buffer) + 1);  // 包含\0
        pthread_mutex_unlock(&print_mutex);
        
        if (bytes_written == -1) {
            perror("发送失败");
            break;
        }
        
        printf("你: ");
        fflush(stdout);
    }
    
    close(write_fd);
    close(read_fd);
    
    // 清理管道文件
    unlink(fifo_a_to_b);
    unlink(fifo_b_to_a);
    
    printf("\n聊天结束\n");
    return 0;
}