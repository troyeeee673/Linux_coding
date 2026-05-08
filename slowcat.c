#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#define CPS		10
#define BUFSIZE         CPS
static volatile int loop = 0;
static void alrm_handler(int s)
{
//	alarm(1);//在这里设置下一次闹钟，否则SIGALRM只会产生一次
	loop =1;
}
int main(int argc, char** argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, pos = 0;
    struct itimerval itv;
    if(argc < 2)
    {
        fprintf(stderr, "...");
        exit(1);
    }
    signal(SIGALRM, alrm_handler);
  //  alarm(1);
  
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &itv, NULL) < 0)
    {
	    perror("seitimer()");
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
	    while(!loop)
		pause() ;
	    loop = 0;
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
