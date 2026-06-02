#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    struct dirent *dirent;
    char type;
    DIR *dir = opendir(argv[1]);
    if (!dir)
    {
        perror("opendir()");
        exit(1);
    }
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
                exit(1);
            }
        }
        if (strncmp(dirent->d_name, ".", 1)) // 相等返回0
        {
            switch (dirent->d_type)
            {
            case 8:
                type = '-';
                break;
            case 10:
                type = 'l';
                break;
            case 4:
                type = 'd';
                break;
            default:
                break;
            }
            printf("%c %ld\t%s\n",type, dirent->d_ino,  dirent->d_name);
            
        }
    }

    closedir(dir);
    exit(0);
}