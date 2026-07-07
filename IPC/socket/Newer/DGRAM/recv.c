#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
    }
    int opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t)sizeof(opt));
    // bind
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(1900);

    socklen_t len = sizeof(addr);
    bind(sd, (struct sockaddr *)&addr, len);
    char buf[1472];

    while (1)
    {
        struct sockaddr_in addr_sender;
        memset(&addr_sender, 0, sizeof(addr_sender));
        len = sizeof(addr_sender);

        ssize_t size = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&addr_sender, &len);
        if (size <= 0)
        {
            close(sd);
            perror("recefrom()");
            exit(1);
        }
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_sender.sin_addr, ip, INET_ADDRSTRLEN);
        int port = ntohs(addr_sender.sin_port);
        char *p = strchr(buf, '\n');
        if(p)
        {
            *p = '\0';
        }
        printf("data recv: %s    from %s:%d   length = %ld\n", buf, ip, port, size);
    }

    close(sd);
}