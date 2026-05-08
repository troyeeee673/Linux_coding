#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEFT    30000000
#define RIGHT   30000200

int main()
{
    
    for(int i = LEFT;i<= RIGHT;i++)
    {
        int mark = 1;
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
