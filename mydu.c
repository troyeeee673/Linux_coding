#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>
#include <stdbool.h>
#define PATHSIZE	1024

//防止死循环
static bool path_noloop(const char *path)
{
	char* pos = strrchr(path, '/');
	if(!pos)
	{
		return true;
	}
	if(strcmp(pos+1, ".") == 0 || strcmp(pos+1, "..") == 0)
		return false;
	return true;

}

static int64_t mydu(const char* path)
{
	struct stat statres;
	char nextpath[PATHSIZE];
	glob_t globres;
	int64_t sum = 0;
	 if(lstat(path, &statres) < 0)
	 {
		 perror("lstat()");
		 exit(1);
	 }
	if(!S_ISDIR( statres.st_mode))
		return statres.st_blocks;
	else
	{
		sum += statres.st_blocks;
		/*strncpy(nextpath, path, PATHSIZE);
		strncat(nextpath, "/*", PATHSIZE);*/

		snprintf(nextpath, PATHSIZE,"%s/*", path);
		int ret = glob(nextpath, GLOB_NOSORT, NULL,&globres);

		if(ret != 0 && ret != GLOB_NOMATCH)
		{
			printf("Error");
			exit(1);
		}
		/*
		strncpy(nextpath, path, PATHSIZE);
		strncat(nextpath, "/.*", PATHSIZE);*/
		snprintf(nextpath,PATHSIZE,"%s/.*",path);
		glob(nextpath ,GLOB_APPEND|GLOB_NOSORT ,NULL, &globres);

		for(int i = 0 ;i< globres.gl_pathc;i++)
		{
			if(path_noloop(globres.gl_pathv[i]))
			sum += mydu(globres.gl_pathv[i]);
		}
		globfree(&globres);
		return sum;
	}
	return 0;
}

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	printf("%ld\n",mydu(argv[1]) / 2);

	exit(0);

}
