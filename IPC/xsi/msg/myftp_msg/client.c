#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "protol2.h"

int get_msg_queue() {
    key_t key = ftok(KEYPATH, KEYPROJ);
    if (key == -1) {
        perror("ftok");
        return -1;
    }
    
    int msgid = msgget(key, 0);
    if (msgid == -1) {
        perror("msgget");
        return -1;
    }
    
    return msgid;
}

int send_path_request(int msgid, const char *path) {
    msg_path_t request;
    request.mtype = MSG_PATH;
    strncpy(request.path, path, PATHMAX - 1);
    request.path[PATHMAX - 1] = '\0';
    
    if (msgsnd(msgid, &request, sizeof(request) - sizeof(long), 0) == -1) {
        perror("msgsnd path");
        return -1;
    }
    
    printf("Requested file: %s\n", path);
    return 0;
}

int receive_and_save_file(int msgid, const char *save_path) {
    // 使用二进制写入模式，并确保缓冲区立即刷新
    FILE *fp = fopen(save_path, "wb");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }
    
    // 禁用缓冲区，确保立即写入
    setbuf(fp, NULL);
    
    ssize_t ret;
    msg_data_t data_packet;
    int total_bytes = 0;
    int packet_count = 0;
    
    while (1) {
        // 接收数据包
        ret = msgrcv(msgid, &data_packet, sizeof(data_packet) - sizeof(long), 
                    MSG_DATA, 0);
        
        if (ret == -1) {
            perror("msgrcv");
            fclose(fp);
            return -1;
        }
        
        printf("Received packet with datalen: %d\n", data_packet.datalen);
        
        // 检查是否为EOT包（datalen == 0）
        if (data_packet.datalen == 0) {
            printf("Received EOT, transfer completed\n");
            break;
        }
        
        // 写入文件数据
        if (data_packet.datalen > 0) {
            size_t written = fwrite(data_packet.data, 1, data_packet.datalen, fp);
            if (written != data_packet.datalen) {
                perror("fwrite");
                fclose(fp);
                return -1;
            }
            
            // 强制刷新缓冲区
            fflush(fp);
            
            total_bytes += written;
            packet_count++;
            printf("Wrote %zu bytes to file (total: %d, packets: %d)\n", 
                   written, total_bytes, packet_count);
        }
    }
    
    // 再次确保所有数据已写入
    fflush(fp);
    
    // 获取文件大小进行验证
    long file_size = ftell(fp);
    fclose(fp);
    
    printf("File saved as: %s (%ld bytes total, %d packets)\n", 
           save_path, file_size, packet_count);
    
    // 验证文件是否真的被创建且有内容
    struct stat st;
    if (stat(save_path, &st) == 0) {
        printf("File on disk size: %ld bytes\n", st.st_size);
        if (st.st_size == 0) {
            fprintf(stderr, "WARNING: File is empty on disk!\n");
            return -1;
        }
    }
    
    return total_bytes > 0 ? 0 : -1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <remote_file_path> <local_save_path>\n", argv[0]);
        exit(1);
    }
    
    const char *remote_path = argv[1];
    const char *local_path = argv[2];
    
    int msgid = get_msg_queue();
    if (msgid == -1) {
        exit(1);
    }
    
    // 发送文件路径请求
    if (send_path_request(msgid, remote_path) == -1) {
        exit(1);
    }
    
    // 接收并保存文件
    if (receive_and_save_file(msgid, local_path) == -1) {
        fprintf(stderr, "Failed to receive file\n");
        exit(1);
    }
    
    return 0;
}