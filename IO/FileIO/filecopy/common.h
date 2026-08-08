#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define MAX_BUFSIZE     4096
#define SERV_PORT       1900
#define SERV_IP         "127.0.0.1"
#define MD5_LEN         16

typedef struct
{
    int name_len;
    long file_size;
    unsigned char md[SHA256_DIGEST_LENGTH];
}FileHeader;

int compute_file_md5(const char *filename, unsigned char *md5_out)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        perror("EVP_MD_CTX_new");
        fclose(fp);
        return -1;
    }

    // 初始化 MD5 上下文
    if (EVP_DigestInit_ex(ctx, EVP_md5(), NULL) != 1) {
        perror("EVP_DigestInit_ex");
        EVP_MD_CTX_free(ctx);
        fclose(fp);
        return -1;
    }

    // 分块读取文件并更新摘要
    unsigned char buffer[8192];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (EVP_DigestUpdate(ctx, buffer, bytes) != 1) {
            perror("EVP_DigestUpdate");
            EVP_MD_CTX_free(ctx);
            fclose(fp);
            return -1;
        }
    }

    // 获取最终摘要
    unsigned int md5_len;
    if (EVP_DigestFinal_ex(ctx, md5_out, &md5_len) != 1) {
        perror("EVP_DigestFinal_ex");
        EVP_MD_CTX_free(ctx);
        fclose(fp);
        return -1;
    }

    EVP_MD_CTX_free(ctx);
    fclose(fp);
    return 0;
}

/**
 * 打印 MD5 值（用于调试）
 */
void print_md5(const unsigned char *md5)
{
    for (int i = 0; i < MD5_LEN; i++) {
        printf("%02x", md5[i]);
    }
    printf("\n");
}

#endif