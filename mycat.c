#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFSIZE     1024

int main(int argc, char** argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, pos = 0;
    if(argc < 2)
    {
        fprintf(stderr, "...");
        exit(1);
    }
   sfd = open(argv[1], O_RDONLY);
   if(sfd < 0)
   {
    perror("open()");
    exit(1);
   }
   

    while(1)
    {
        len = read(sfd, buf, BUFSIZE);
        if(len < 0)
        {
            perror("read()");
            break;
        }
        if(len == 0)
        {
            break;
        }
        while(len > 0)
        {
          int  ret = write(dfd, buf + pos, len);
            if(ret <0)
            {
                perror("write()");
                exit(1);
            }
            len -= ret;
            pos += ret;

        }
        
    }

    close(sfd);
    exit(0);

}
