#define __GNU_SORUCE
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
	sigset_t set, oldset, saveset;
	//signal(SIGINT, SIG_IGN);
	signal(SIGINT, int_handler);//如果按住ctrl+c不放，就会打断阻塞系统调用sleep,导致它会立即返回，不会睡眠1秒钟，导致快速打印
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigprocmask(SIG_UNBLOCK, &set, &saveset);//先保存当前的状态
	for(int j = 0 ;j < 1000; j++)
	{	
		//sigprocmask(SIG_BLOCK,&set, &oldset); 
		sigprocmask(SIG_BLOCK, &set, NULL);
		for(int i =0 ;i< 10 ;i++)
		{
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);
		//sigprocmask(SIG_SETMASK, &oldset, NULL);
		sigprocmask(SIG_UNBLOCK, &set, NULL);
		// ← 如果有待处理的SIGINT，会在这里立即处理
	}

	sigprocmask(SIG_SETMASK, &saveset, NULL);//恢复信号集
	exit(0);

}
