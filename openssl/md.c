//使用旧版本的API
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>


void to_md5(char* str)
{
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;
    MD5_Init(&ctx);//初始化
    MD5_Update(&ctx, str, strlen(str));//更新上下文，可以多次分片执行
    MD5_Final(hash, &ctx);//生成最终摘要信息,存入目标hash中

    printf("MD5:\t");
    //将16位二进制(16bit)结果转为16进制(32bit)
    for(int i = 0 ;i < MD5_DIGEST_LENGTH ;i ++)
    {
        printf("%02x", hash[i]);

    }
    printf("\n");

}

void file_to_sha128(char* filename)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    FILE* fp = fopen(filename, "rb");
    if(!fp)
    {
        perror("fopen()");
        return;
    }

    //这里得到文件大小，但是容易出现内存分配不足的风险
    long int file_size = 0;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    SHA_CTX ctx;
    SHA1_Init(&ctx);//初始化
    //每次读一个块大小，多次处理，避免一次性分配大内存
    /*
    char buf[4096];
    int n;
    while((n = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        SHA1_Update(&ctx, buf, n);//更新
    }
    */
    char *file_buf = (char*)malloc(sizeof(char) * file_size);

    size_t file_read = fread(file_buf, 1, file_size, fp);
    if(file_read != file_size)
    {
        free(file_buf);
        fclose(fp);
        exit(1);
    }
    
    SHA1_Update(&ctx, file_buf, file_size);//更新上下文，可以多次分片执行
    SHA1_Final(hash, &ctx);//生成最终摘要信息,存入目标hash中

    printf("SHA128:\t");
    //将16位二进制(16bit)结果转为16进制(40bit)
    for(int i = 0 ;i < SHA_DIGEST_LENGTH ;i ++)
    {
        printf("%02x", hash[i]);

    }
    printf("\n");
    fclose(fp);
}

void to_sha256(char* str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, str, strlen(str));
    SHA256_Final(hash, &ctx);

    printf("SHA256:\t");
    //将16位二进制(32bit)结果转为16进制(64bit)
    for(int i = 0 ;i < SHA256_DIGEST_LENGTH ;i ++)
    {
        printf("%02x", hash[i]);

    }
    printf("\n");


}

int main()
{
    char* str = "hello openSSL";
    to_md5(str);
    to_sha256(str);
    file_to_sha128("md");

    exit(0);
}