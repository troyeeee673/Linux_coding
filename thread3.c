#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define LEFT    30000000
#define RIGHT   30000200

int main()
{
    pid_t pid;
    int mark;
    for(int i = LEFT;i<= RIGHT;i++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if(pid == 0)
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
                printf("%d is a primer\n", i);
            exit(0);//必须加这句，让子进程退出，否则子进程又会fork出子进程
        }
    }

    for(int i = LEFT; i <= RIGHT;i++)
    {
	    wait(NULL);
    }
    exit(0);

}
