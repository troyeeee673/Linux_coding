#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
	pid_t pid;
	puts("Begin");
	fflush(NULL);//在执行fork()前都要注意刷新缓冲区

	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}

	if(pid == 0)
	{
		execl("/bin/date", "date", "+%s", NULL);
		perror("execl()");//exec函数族只有再出错的情况下返回，因为返回值就算接收了也无法使用
		
		exit(1);

	}

	wait(NULL);


	puts("End");
	exit(0);
}
