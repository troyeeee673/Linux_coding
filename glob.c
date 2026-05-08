#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>  

#define PAT     "/etc/a*.conf"  

int errfunc_(const char *errpath, int errno_val)
{
    puts(errpath);
    fprintf(stderr, "ERROR MSG:%s\n", strerror(errno_val));
    return 0;
}

int main(int argc, char **argv)
{
    glob_t globres;
    int err = glob(PAT, 0, NULL, &globres); 
    
    if(err)
    {
        printf("Error code = %d\n", err);
        exit(0);
    }
    
    // 输出匹配到的文件
    for(int i = 0; i < globres.gl_pathc; i++)
        puts(globres.gl_pathv[i]);
    
    // 释放 glob 占用的内存
    globfree(&globres);
    exit(0);
}
