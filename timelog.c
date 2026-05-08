#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BUFFERSIZE	1024
int main(int argc, char ** argv)
{

	FILE* fp;
	time_t stamp;
	struct tm *tm;
	char buf[BUFFERSIZE];
	int count = 0;
	fp = fopen(argv[1], "a+");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}
	while(fgets(buf, BUFFERSIZE, fp) != NULL)
		count ++;//确保下次count数不是从0开始

	while(1)
	{
		time(&stamp);
		tm = localtime(&stamp);
		fprintf(fp, "%-4d%d-%02d-%02d %02d:%02d:%02d\n",++count,\
			       	tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,\
				tm->tm_hour, tm->tm_min, tm->tm_sec);
		fflush(fp);
		sleep(1);
	}



	fclose(fp);


	exit(0);
}
