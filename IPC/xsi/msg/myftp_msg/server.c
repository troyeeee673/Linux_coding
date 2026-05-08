#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "protol2.h"

// 获取消息队列ID
int get_msg_queue() {
    key_t key = ftok(KEYPATH, KEYPROJ);
    if (key == -1) {
        perror("ftok");
        return -1;
    }
    
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        return -1;
    }
    
    return msgid;
}

// 发送文件数据包
int send_data_packet(int msgid, long mtype, const char *data, int datalen) {
    msg_data_t packet;
    packet.mtype = mtype;
    packet.datalen = datalen;
    
    if (datalen > 0 && data != NULL) {
        memcpy(packet.data, data, datalen);
    }
    
    if (msgsnd(msgid, &packet, sizeof(packet) - sizeof(long), 0) == -1) {
        perror("msgsnd data");
        return -1;
    }
    
    return 0;
}

// 发送结束包
int send_eot_packet(int msgid, long mtype) {
    msg_eot_t eot;
    eot.mtype = mtype;
    
    if (msgsnd(msgid, &eot, sizeof(eot) - sizeof(long), 0) == -1) {
        perror("msgsnd eot");
        return -1;
    }
    
    return 0;
}

// 处理文件传输请求
void handle_file_request(int msgid, const msg_path_t *request) {
    printf("Client requested file: %s\n", request->path);
    
    // 打开文件
    int fd = open(request->path, O_RDONLY);
    if (fd == -1) {
        perror("open file");
        // 发送空数据包表示错误
        send_eot_packet(msgid, MSG_EOT);
        return;
    }
    
    // 读取并发送文件内容
    char buffer[DATAMAX];
    ssize_t bytes_read;
    
    while ((bytes_read = read(fd, buffer, DATAMAX)) > 0) {
        if (send_data_packet(msgid, MSG_DATA, buffer, bytes_read) == -1) {
            break;
        }
        printf("Sent %ld bytes\n", bytes_read);
    }
    
    close(fd);
    
    // 发送结束包
    send_eot_packet(msgid, MSG_EOT);
    printf("File transfer completed\n");
}

int main() {
    int msgid = get_msg_queue();
    if (msgid == -1) {
        exit(1);
    }
    
    printf("FTP Server started, waiting for requests...\n");
    
    msg_path_t request;
    
    while (1) {
        // 接收客户端请求
        ssize_t ret = msgrcv(msgid, &request, sizeof(request) - sizeof(long), 
                            MSG_PATH, 0);
        
        if (ret == -1) {
            if (errno == EINTR) continue;
            perror("msgrcv");
            break;
        }
        
        // 处理文件请求
        handle_file_request(msgid, &request);
    }
    
    // 清理消息队列（可选）
    // msgctl(msgid, IPC_RMID, NULL);
    
    return 0;
}