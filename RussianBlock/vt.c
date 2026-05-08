#include <stdio.h>
#include <unistd.h>  // for sleep

int main()
{
    printf("\033[10;10Hhello");
    fflush(stdout);   // 强制刷新输出
    printf("\n");     // 最后换行，避免影响提示符
    
    return 0;
}