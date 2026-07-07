#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    char msg[1024];
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        perror("sender:socket()");
        exit(1);
    }
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(1900);

    while (1)
    {
        memset(msg, 0, sizeof(msg));
        printf(":");
        fgets(msg, 1024, stdin);
        ssize_t size = sendto(sd, msg, sizeof(msg), 0, (struct sockaddr *)&addr, len);
        if (size <= 0)
        {
            perror("sendto()");
            exit(1);
        }
        if(strncmp(msg, "bye", 3) == 0)
        {
            printf("close connection\n");
            break;
        }

        char *p = strchr(msg, '\n');
        if(p)
        {
            *p = '\0';
        }
        printf("send:%s, len = %ld\n", msg, size);
    }

    close(sd);
    return 0;
}