#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h> //基本输入输出,类似于FILE

// 返回值是指针类型(防止函数结束后内存被释放)，意味着函数内部有动态内存分配，后需要注意进行释放
// 实现将传入字符串进行base64编码后进行返回
char *encode(char *str)
{
    BIO *b64 = BIO_new(BIO_f_base64()); // BIO_f_**表示过滤器
    BIO *bio = BIO_new(BIO_s_mem());    // BIO_s_表示数据源

    // 给bio加上base64过滤器
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // 设置flag，设置bio在进行base64编码时不换行

    BIO_write(bio, str, strlen(str)); // 从内存读数据str,经过过滤器到达bio

    BIO_flush(bio); // 刷新缓冲区
    BUF_MEM *buf;   // 内存缓冲区
    BIO_get_mem_ptr(bio, &buf);

    // 动态分配的堆空间
    char *data = malloc(buf->length + 1);
    memcpy(data, buf->data, buf->length);
    data[buf->length] = '\0';

    BIO_free_all(bio);

    return data;
}

char *decode(char *data, int *len)
{
    BIO *b64 = BIO_new(BIO_f_base64());

    int input_len = strlen(data);

    BIO *bio = BIO_new_mem_buf(data, input_len);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    char *buf = malloc(input_len);
    *len = BIO_read(bio, buf, input_len);
    BIO_free_all(bio);

    return buf;
}


void encode_file(char* src, char* dest)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    //读源文件
    BIO *src_file = BIO_new_file(src, "rb");
    //写目标文件
    BIO *dest_file = BIO_new_file(dest, "w");

    dest_file = BIO_push(b64, dest_file);

    char buf[1024*4];
    int n;
    while((n = BIO_read(src_file, buf, sizeof(buf))) > 0)
    {
        BIO_write(dest_file, buf, n);
    }

    BIO_flush(dest_file);
    BIO_free_all(dest_file);
    BIO_free_all(src_file);
    
}

void decode_file(char* src, char* dest)
{
    BIO *b64 = BIO_new(BIO_f_base64());

    //读源文件
    BIO *src_file = BIO_new_file(src, "r");
    //写目标
    BIO *dest_file = BIO_new_file(dest, "wb");

    src_file = BIO_push(b64, src_file);

    char buf[1024 * 4];
    int n;
    while((n = BIO_read(src_file, buf, sizeof(buf))) > 0)
    {
        BIO_write(dest_file, buf, n);
    }

    BIO_flush(dest_file);
    BIO_free_all(dest_file);
    BIO_free_all(src_file);
}


int main()
{
    //文本的编码与解码
    //SGVsbG8gQmFzZTY0
    char *str = "Hello Base64";
    char *base64 = encode(str);

    printf("%s\n", base64);

    //释放base64所指的空间
     free(base64);

    char *data = "SGVsbG8gQmFzZTY0";
    int len = 0;
    char *text_origin = decode(data, &len); // 回写解码后字符串长度
    printf("%s\n", text_origin);

    free(text_origin);

    //文件的编码与解码

    //编码
    encode_file("codex.txt", "codex_Base64.txt");

    //解码
    decode_file("codex_Base64.txt", "codex_decoded.txt");
    return 0;
}