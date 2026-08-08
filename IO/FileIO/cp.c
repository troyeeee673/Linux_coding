#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>


int main(int argc, char ** argv)
{
    if(argc != 3)
    {
        perror("Usage!\n");
        return EXIT_FAILURE;
    }
    int fd_in, fd_out;
    fd_in = open(argv[1], O_RDONLY);
    if(fd_in < 0)
    {
        perror("open fd_in");
        return EXIT_FAILURE;
    }
    fd_out = open(argv[2], O_RDWR | O_CREAT | O_APPEND, 0600);
    if(fd_out < 0)
    {
        perror("open fd_out");
        close(fd_in);
        return EXIT_FAILURE;
    }
    char ch;
    char buf[2048] = {0};
    int len;
    // while(read(fd_in, &ch, 1))
    // {
    //     write(fd_out, &ch, 1);
    // }

    while(len = read(fd_in, buf, sizeof(buf)))
    {
        write(fd_out, buf, len);
    }

    fprintf(stdout, "copy successfully\n");

    close(fd_in);
    close(fd_out);
    return EXIT_SUCCESS;
}