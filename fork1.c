#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main()
{

	pid_t pid;
	printf("%d Begin\n", getpid());
	
	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}

	if(pid == 0)
	{
		printf("%d child is working\n", getpid());
	}

	else 
	{
		printf("%d parent is working\n", getpid());
	}

	printf("%d End\n", getpid());
	exit(0);
	
}
