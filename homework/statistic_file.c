#define _DEFAULT_SOURCE
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

typedef struct Count
{
    long dir_count;
    long file_count;
    long code_count;
} Count;

long code_counter(char *filepath)
{
    int ch;
    long code_count = 0;
    FILE *fp = fopen(filepath, "r");
    if (!fp)
    {
        fprintf(stderr, "file:%s\n", filepath);
        perror("fopen() in code_counter");
        return 0;  // 不要 exit，返回 0 继续
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
            code_count++;
    }
    code_count++;
    fclose(fp);
    return code_count;
}

void *show_dir(void *arg)
{
    char *dir_name = (char *)arg;
    int dir_count = 0, file_count = 0;
    long code_count = 0;
    pthread_t *tids = NULL;
    long tid_count = 0;

    Count *rtval_curr = malloc(sizeof(Count));
    rtval_curr->code_count = 0;
    rtval_curr->dir_count = 0;
    rtval_curr->file_count = 0;

    char full_path[1024];
    struct dirent *dirent;
    DIR *dir = opendir(dir_name);
    if (!dir)
    {
        fprintf(stderr, "opendir(): %s\n", dir_name);
        free(rtval_curr);  // 记得释放
        return NULL;
    }

    printf("\n目录: %s\n", dir_name);
    errno = 0;
    while ((dirent = readdir(dir)) != NULL)
    {
        if (strcmp(dirent->d_name, ".") == 0 ||
            strcmp(dirent->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s",
                 dir_name, dirent->d_name);

        // 用 stat 获取可靠的文件类型
        struct stat st;
        if (stat(full_path, &st) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISREG(st.st_mode))  // 普通文件
        {
            file_count++;
            size_t len = strlen(full_path);
            // 检查后缀：.c 或 .h
            if (len > 2 && full_path[len - 1] == 'c' && full_path[len - 2] == '.')
                code_count += code_counter(full_path);
            else if (len > 2 && full_path[len - 1] == 'h' && full_path[len - 2] == '.')
                code_count += code_counter(full_path);
            printf("- %ld\t%s\n", st.st_ino, dirent->d_name);
        }
        else if (S_ISDIR(st.st_mode))  // 目录
        {
            dir_count++;
            pthread_t tid;
            // strdup 复制路径，避免 full_path 被覆盖
            char *path_copy = strdup(full_path);
            if (!path_copy)
            {
                perror("strdup");
                continue;
            }
            pthread_create(&tid, NULL, show_dir, path_copy);
            tids = realloc(tids, sizeof(pthread_t) * (tid_count + 1));
            tids[tid_count++] = tid;
            printf("d %ld\t%s\n", st.st_ino, dirent->d_name);
        }
        else if (S_ISLNK(st.st_mode))
        {
            printf("l %ld\t%s\n", st.st_ino, dirent->d_name);
        }

        errno = 0;
    }

    if (errno != 0)
    {
        perror("readdir()");
    }

    closedir(dir);

    // 先把本层的统计结果存入
    rtval_curr->code_count = code_count;
    rtval_curr->dir_count = dir_count;
    rtval_curr->file_count = file_count;

    // 等待所有子线程并累加结果
    for (int i = 0; i < tid_count; i++)
    {
        Count *rtval_child = NULL;
        int ret = pthread_join(tids[i], (void **)(&rtval_child));
        if (ret == 0 && rtval_child != NULL)
        {
            rtval_curr->code_count += rtval_child->code_count;
            rtval_curr->dir_count += rtval_child->dir_count;
            rtval_curr->file_count += rtval_child->file_count;
            free(rtval_child);
        }
    }

    free(tids);
    pthread_exit(rtval_curr);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "用法: %s <目录>\n", argv[0]);
        exit(1);
    }

    pthread_t tid;
    char *start_path = strdup(argv[1]);  // 复制一份，避免潜在问题
    pthread_create(&tid, NULL, show_dir, start_path);

    Count *rtval = NULL;
    pthread_join(tid, (void **)(&rtval));

    if (rtval)
    {
        printf("\n===== 统计结果 =====\n");
        printf("文件数：%ld\n", rtval->file_count);
        printf("目录数：%ld\n", rtval->dir_count);
        printf("代码行数：%ld\n", rtval->code_count);
        free(rtval);
    }

    free(start_path);
    return 0;
}