#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

int main()
{
    // char buf[16] = {0};
    // int n = read(0, buf, 16);
    // if(n < 0)
    // {
    //     write(2, "error", 6);
    //     return 1;
    // }
    // if(write(1, buf, n) < 0)
    // {
    //     write(2, "error", 6);
    //     return 1;
    // }
    // return 0;
    int fd = open("abc.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    printf("%d\n", fd);
    if(write(fd, "hello", 6) != 6)
    {
        perror("error");
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}