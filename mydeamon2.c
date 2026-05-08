#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#define FILENAME	"/tmp/out"

static int daemonize(void)
{
	pid_t pid;
	int fd;
	pid = fork();
	if(pid < 0)
	{
		
		return 1;
	}
	if(pid > 0)
	{
		exit(0);//让 shell 认为程序已经执行完毕，子进程成为"孤儿"由 init 收养。
	}

	fd = open("/dev/null", O_RDWR);
	if(fd < 0)
	{
		
		return -1;
	}
	dup2(fd,0);
	dup2(fd,1);
	dup2(fd,2);//关闭三个标准流，重定向标准 I/O 到 /dev/null
	//守护进程脱离终端，不能读写终端，所有标准 I/O 都被丢弃。
	if(fd > 2)
		close(fd);

	//子进程成为新会话的首进程
	//子进程成为新进程组的组长
	//彻底脱离控制终端
	setsid();
	
	chdir("/");//将这个daemon进程的工作目录切换到根目录，防止daemon进程运行在一个将要umount的设备上
	//umask(0);//如果确认这个进程不会产生文件了，就可以设置umask为0
	return 0;
}
int main()
{
	int i = 1;
	openlog("mydaemon", LOG_PID,LOG_DAEMON);//初始化 syslog（只需调用一次）
	if(daemonize())
	{
		syslog(LOG_ERR,"daemonize failed");//提交日志
		exit(1);
	}
	else
	{
		syslog(LOG_INFO, "daemonize() succeed!");
	}
	FILE* fp = fopen(FILENAME, "w");
	if(fp == NULL)
	{
		syslog(LOG_ERR,"fopen():%s", strerror(errno));
		exit(1);
	}
	syslog(LOG_INFO,"%s was opended.", FILENAME);
	while(i)
	{
		fprintf(fp, "%d\n",i++);
		fflush(fp);
		sleep(1);
	}
	
	fclose(fp);
	closelog();
	exit(0);
}
