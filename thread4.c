#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define LEFT    30000000
#define RIGHT   30000200
#define N	3 //进程数
int main()
{
    pid_t pid;
    int mark;
    for(int n = 0;n <N;n++)
    {
	    pid = fork();
	    if(pid < 0)
	    {
		    perror("fork()");
		    //应该要写个循环将以创建的资源释放
		    exit(1);
	    }
	    if(pid == 0)
	    {
   	    for(int i = LEFT + n;i<= RIGHT;i += N)
    	    {
        
            	mark = 1;
            	for(int j = 2;j<i/2;j++)
            	{
                	if(i %j == 0)
                	{
                    		mark = 0;
                    		break;
                	}
            	}
            	if(mark)
                	printf("[%d]%d is a primer\n",n, i);
            	
        
    	    }
	    exit(0);
	    }
    }		

    for(int i = 0; i < N;i++)
    {
	    wait(NULL);
    }
    exit(0);

}
