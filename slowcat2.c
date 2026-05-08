#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#define CPS		10
#define BUFSIZE         CPS
#define BURST		100
static volatile sig_atomic_t  token = 0;//确保对token进行的操作是由一条指令完成的，避免了多处指令对token同时操作
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
    if(argc < 2)
    {
        fprintf(stderr, "...");
        exit(1);
    }
    signal(SIGALRM, alrm_handler);
    alarm(1);
   sfd = open(argv[1], O_RDONLY);
   if(sfd < 0)
   {
    perror("open()");
    exit(1);
   }
   

    while(1)
    {
	    while(token <= 0)
		pause() ;
	    token--;
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
    exit(0);

}
