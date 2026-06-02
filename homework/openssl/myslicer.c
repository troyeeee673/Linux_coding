//对文件进行分片，按片进行md5加密，生成消息摘要，并将消息摘要记录在指定文件中，每一行是一个分片的md5结果值，最后一行是整个文件的md5值
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "slice.h"

#define BUFSIZE 64
#define MAX_SLICE   100
#define HASH_STR_LEN  (SHA256_DIGEST_LENGTH * 2 + 1)  

char split_result[MAX_SLICE][12];
int slice_count = 0;

//对文件进行分片，并将结果文件名保存到split_result中，返回设置分片数量
void slice(char * filename)
{
    static char result[HASH_STR_LEN];
    char buf[BUFSIZE];
    int len;
    char split[32];
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("fopen()");
        return;
    }

    while ((len = fread(buf, 1, BUFSIZE, fp)))
    {
        sprintf(split, "split_%d.txt", slice_count);
        strcpy(split_result[slice_count], split);
        FILE *fpd = fopen(split, "wb+");
        if (!fpd)
        {
            fclose(fp);
            perror("fopen()");
            return;
        }
        fwrite(buf, 1, len, fpd);
        fclose(fpd);
        slice_count++;
    }
    fclose(fp);
}

char* file_to_md5(char* filename)
{
    static char result[HASH_STR_LEN]; 
    char *ptr = result;  // 指向当前位置
    unsigned char hash[MD5_DIGEST_LENGTH];
    FILE* fp = fopen(filename, "rb");
    if(!fp)
    {
        perror("fopen()");
        return NULL;
    }

    //这里得到文件大小，但是容易出现内存分配不足的风险
    long int file_size = 0;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    MD5_CTX ctx;
    MD5_Init(&ctx);//初始化
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
    
    MD5_Update(&ctx, file_buf, file_size);//更新上下文，可以多次分片执行
    MD5_Final(hash, &ctx);//生成最终摘要信息,存入目标hash中

    //将16位二进制(16bit)结果转为16进制(32bit)
    for(int i = 0 ;i < MD5_DIGEST_LENGTH ;i ++)
    {
        ptr += sprintf(ptr, "%02x", hash[i]);//防止覆盖之前的内容
    }
    *ptr = '\0';
    fclose(fp);
    return result;
    
}

char* evp_file_to_md5(char* filename)
{
    static char result[HASH_STR_LEN]; 
    char *ptr = result;  // 指向当前位置
    unsigned char hash[MD5_DIGEST_LENGTH];
    FILE* fp = fopen(filename, "rb");
    if(!fp)
    {
        perror("fopen()");
        return NULL;
    }

    
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_md5(), NULL);//第三个参数为引擎，如果为空则使用默认的引擎

    char buf[BUFSIZE];
    int n;
    while((n = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        EVP_DigestUpdate(ctx, buf, n);
    }

    int len;//回写长度
    EVP_DigestFinal_ex(ctx, hash, &len);

    EVP_MD_CTX_free(ctx);

    //将16位二进制(16bit)结果转为16进制(32bit)
    for(int i = 0 ;i < MD5_DIGEST_LENGTH ;i ++)
    {
        ptr += sprintf(ptr, "%02x", hash[i]);//防止覆盖之前的内容
    }
    *ptr = '\0';
    fclose(fp);
    return result;
    
}

//将结果写入meta文件中
void write_to_meta(char* filename, char* meta_name)
{
    slice(filename);
    char* str_receive;
    FILE* fp = fopen(meta_name, "wb+");
    if(!fp)
    {
        fclose(fp);
        perror("fopen()");
        return ;
    }
    for(int i = 0 ;i < slice_count; i++)
    {
        str_receive = evp_file_to_md5(split_result[i]);
        fwrite(str_receive, 1, MD5_DIGEST_LENGTH * 2, fp);
        fwrite("\n", 1, 1, fp);
    }
    str_receive = evp_file_to_md5(filename);
    fwrite(str_receive, 1, MD5_DIGEST_LENGTH * 2, fp);
    fclose(fp);
}


int main(int argc, char** argv)
{
    write_to_meta("test.txt", "meta.txt");

}