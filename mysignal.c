#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void int_handler(int s)
{
	write(1, "!", 1);
}
int main()
{
	//signal(SIGINT, SIG_IGN);
	signal(SIGINT, int_handler);//如果按住ctrl+c不放，就会打断阻塞系统调用sleep,导致它会立即返回，不会睡眠1秒钟，导致快速打印
	for(int i =0 ;i< 10 ;i++)
	{
		write(1, "*", 1);
		sleep(1);
	}
	exit(0);

}
