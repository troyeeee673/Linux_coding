//使用旧版本的API
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

void evp_to_md5(char* str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();//创建上下文
    EVP_DigestInit_ex(ctx, EVP_md5(), NULL);//第三个参数为引擎，如果为空则使用默认的引擎
    EVP_DigestUpdate(ctx, str, strlen(str));
    int len;//回写长度
    EVP_DigestFinal_ex(ctx, hash, &len);

    EVP_MD_CTX_free(ctx);
    printf("MD5:\t");
    //将16位二进制(16bit)结果转为16进制(32bit)
    for(int i = 0 ;i < MD5_DIGEST_LENGTH ;i ++)
    {
        printf("%02x", hash[i]);

    }
    printf("\n");
}

int main()
{
    char* str = "hello openSSL";
    evp_to_md5(str);
    exit(0);
}