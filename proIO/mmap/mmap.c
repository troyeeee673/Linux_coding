//随意指定一个文件，告诉这个文件中有多少个字符a

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
    int fd;
    struct stat stat_ret;
    char * str;
    int i , count = 0;

    if(argc < 2)
    {
        fprintf(stderr, "Usage...");
        //printf("Usage...");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if(fd < 0)
    {
        perror("open()");
        exit(1);
    }
    if(fstat(fd, &stat_ret) < 0)
    {
        perror("fstat()");
        exit(1);
    }

    str = mmap(NULL, stat_ret.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(str == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    close(fd);

    for(i = 0 ;i < stat_ret.st_size;i++)
    {
        if(str[i] == 'a')
        {
            count++;
        }
    }

    printf("%d\n", count);

    munmap(str, stat_ret.st_size);


    exit(0);
}