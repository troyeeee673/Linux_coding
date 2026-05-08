#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define LEFT    30000000
#define RIGHT   30000200

int main()
{
    for(int i = LEFT; i <= RIGHT; i++)
    {
        int mark = 1;  // 每次循环重置
        
        // 特殊处理小于2的数
        if(i < 2) {
            mark = 0;
        }
        // 单独处理偶数
        else if(i == 2) {
            mark = 1;
        }
        else if(i % 2 == 0) {
            mark = 0;
        }
        else {
            // 只需检查奇数因子，到 sqrt(i) 即可
            int limit = (int)sqrt(i);
            for(int j = 3; j <= limit; j += 2)
            {
                if(i % j == 0)
                {
                    mark = 0;
                    break;
                }
            }
        }
        
        if(mark)
            printf("%d is a primer\n", i);  // 添加换行
    }
    
    exit(0);
}
