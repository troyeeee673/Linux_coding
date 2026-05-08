#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "mytbf.h"
#define CPS		10
#define BUFSIZE         1024
#define BURST		100
static volatile int token = 0;
static void alrm_handler(int s)
{
	alarm(1);//在这里设置下一次闹钟，否则SIGALRM只会产生一次
	token++;
	if(token > BURST)
	{
		token = BURST;
	}
}
int main(int argc, char** argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, pos = 0;
    mytbf_t *tbf;
    int size;
    if(argc < 2)
    {
        fprintf(stderr, "...");
        exit(1);
    }

   tbf =  mytbf_init(CPS, BURST);
   if(tbf == NULL)
   {
	fprintf(stderr, "mytbf_init() failed");
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
	size = mytbf_fetchToken(tbf, BUFSIZE);
	if(size < 0)
	{
		fprintf(stderr, "mybtf_fetchToken(): %s",strerror(abs(size)));
		exit(1);
	}
        len = read(sfd, buf, size);
        if(len < 0)
        {
            perror("read()");
            break;
        }
        if(len == 0)
        {
            break;
        }
	if(size - len > 0)
		mytbf_returnToken(tbf, size - len);
	pos = 0;
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
    mytbf_destroy(tbf);
    exit(0);

}
