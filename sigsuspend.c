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
	sigprocmask(SIG_BLOCK,&set, &oldset);
	for(int j = 0 ;j < 1000; j++)
	{	
//		sigprocmask(SIG_BLOCK, &set, NULL);
		for(int i =0 ;i< 10 ;i++)
		{
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);
		sigsuspend(&oldset);//解除某个信号的阻塞状态后立马进入等待状态，等待一个信号的到来，相当于下面几句话的原子操作
		/*
		sigset_t tmpset;
		sigprocmask(SIG_SETMASK, &oldset, &tmpset);//解除阻塞
		pause();//这里就能接收到SIG_INT信号了
		sigprocmask(SIG_SETMASK, &tmpset, NULL);//相当于又恢复到了&set阻塞状态
		*/
//		sigprocmask(SIG_UNBLOCK, &set, NULL);
	}

	sigprocmask(SIG_SETMASK, &saveset, NULL);
	exit(0);

}
