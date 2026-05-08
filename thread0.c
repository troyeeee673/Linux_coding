#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEFT    30000000
#define RIGHT   30000200

int main()
{
    int mark = 1;
    for(int i = LEFT;i<= RIGHT;i++)
    {
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
    }
    exit(0);
}
//这个程序谨慎运行，会创建不止200个进程，因为子进程没有退出
