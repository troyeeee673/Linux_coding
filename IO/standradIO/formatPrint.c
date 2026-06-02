#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{

    //fprintf(),输出到流（文件）中
    FILE*fp = fopen("test1.txt", "r+");
    fprintf(fp, "name: %s\tage:%d\n", "Cassie", 21);
    //sprintf();输出到字符串中
    char text[128];
    sprintf(text, "name: %s\tage:%d\n", "Cassie", 21);
    printf("%s", text);

    //snprintf();
    snprintf(text, 50, "name: %s\tage:%d\n", "Cassie", 21);
    printf("%s", text);
    fclose(fp);
    exit(0);
}