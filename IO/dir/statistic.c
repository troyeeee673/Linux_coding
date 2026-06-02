#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

static long dir_count = 0, file_count = 0, code_count = 0;

long code_counter(char *filepath)
{
    int ch, code_count = 0;
    FILE *fp = fopen(filepath, "r");
    if (!fp)
    {
        fprintf(stderr, "file:%s", filepath);
        perror("fopen() in code_counter");
        exit(1);
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

void show_dir(const char *dir_name)
{
    char type;
    char full_path[1024]; // 用于构建完整路径
    struct dirent *dirent;
    DIR *dir = opendir(dir_name);
    if (!dir)
    {
        fprintf(stderr, "opendir():%s", dir_name);
        return; // 不要 exit，让调用者继续
    }

    printf("\n目录: %s\n", dir_name); // 显示当前目录名
    errno = 0;
    while (1)
    {
        dirent = readdir(dir);
        if (dirent == NULL)
        {
            if (errno == 0)
            {
                printf("读到目录结尾\n");
                break;
            }
            else
            {
                closedir(dir);
                perror("readdir()");
                break;
            }
        }
        if (strcmp(dirent->d_name, ".") == 0 ||
            strcmp(dirent->d_name, "..") == 0)
            continue;

        // 构建完整路径
        snprintf(full_path, sizeof(full_path), "%s/%s",
                 dir_name, dirent->d_name);
        {
            switch (dirent->d_type)
            {
            case DT_REG:
                type = '-';
                file_count++;
                if ((full_path[strlen(full_path) - 1] == 'c' || full_path[strlen(full_path) - 1] == 'h') &&
                    full_path[strlen(full_path) - 2] == '.')
                    code_count += code_counter(full_path);
                break;
            case DT_DIR:
                type = 'd';
                dir_count++;
                show_dir(full_path);
                break;
            case DT_LNK: // 这里没有处理符号链接
                type = 'l';
                break;

            default:
                break;
            }
            printf("%c %ld\t%s\n", type, dirent->d_ino, dirent->d_name);
        }
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    show_dir(argv[1]);
    printf("文件数：%ld", file_count);
    printf("目录数：%ld", dir_count);
    printf("代码行数：%ld", code_count);
    exit(0);
}