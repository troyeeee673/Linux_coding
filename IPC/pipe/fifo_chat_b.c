#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <errno.h>

#define BUFFER_SIZE 256

int main() {
    const char* fifo_a_to_b = "/tmp/fifo_a_to_b";
    const char* fifo_b_to_a = "/tmp/fifo_b_to_a";
    
    printf("\033[35m聊天程序B（等待A连接...）\033[0m\n");
    
    // 创建两个管道（可能已经创建）
    mkfifo(fifo_a_to_b, 0666);
    mkfifo(fifo_b_to_a, 0666);
    
    // B的打开顺序和A相反
    printf("等待A连接...\n");
    int read_fd = open(fifo_a_to_b, O_RDONLY);
    if (read_fd == -1) {
        perror("打开读管道失败");
        return 1;
    }
    
    int write_fd = open(fifo_b_to_a, O_WRONLY);
    if (write_fd == -1) {
        perror("打开写管道失败");
        close(read_fd);
        return 1;
    }
    
    printf("\033[32m已连接！开始聊天（输入 'exit' 退出）\033[0m\n");
    
    char buffer[BUFFER_SIZE];
    fd_set read_fds;
    int max_fd = (read_fd > STDIN_FILENO) ? read_fd : STDIN_FILENO;
    
    printf("你: ");
    fflush(stdout);
    
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(read_fd, &read_fds);
        
        int ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        
        if (ret == -1) {
            perror("select failed");
            break;
        }
        
        // 检查管道数据
        if (FD_ISSET(read_fd, &read_fds)) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t bytes_read = read(read_fd, buffer, BUFFER_SIZE - 1);
            
            if (bytes_read > 0) {
                printf("\r\033[33m对方: %s\033[0m\n", buffer);
                printf("你: ");
                fflush(stdout);
            } else {
                printf("\r\033[31m对方已断开连接\033[0m\n");
                break;
            }
        }
        
        // 检查标准输入
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                break;
            }
            
            buffer[strcspn(buffer, "\n")] = '\0';
            
            if (strlen(buffer) == 0) {
                printf("你: ");
                fflush(stdout);
                continue;
            }
            
            if (strcmp(buffer, "exit") == 0) {
                break;
            }
            
            if (write(write_fd, buffer, strlen(buffer) + 1) == -1) {
                perror("发送失败");
                break;
            }
            
            printf("你: ");
            fflush(stdout);
        }
    }
    
    close(write_fd);
    close(read_fd);
    unlink(fifo_a_to_b);
    unlink(fifo_b_to_a);
    
    printf("聊天结束\n");
    return 0;
}