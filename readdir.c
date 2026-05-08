#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#define PAT	"/etc"

int main()
{
	DIR * dp;//目录指针
	struct dirent * cur;

	dp = opendir(PAT);
	if(dp == NULL)
	{
		perror("opendir()");
		exit(0);
	}

	while((cur = readdir(dp)) != NULL)
		puts(cur->d_name);	
	
	closedir(dp);
	exit(0);
}
