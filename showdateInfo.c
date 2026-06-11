#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

/*
 *  -y:year
 *  -m:month
 *  -d:day
 *  -H:hour
 *  -M:min
 *  -S:sec
 *
 *
 * */
#define TIMESTRSIZE	1024
#define FMTSTRSIZE	1024
int main(int argc, char ** argv)
{

	FILE* fp = stdout;
	time_t stamp;
	struct tm * tm;
	char timestr[TIMESTRSIZE];
	int c;
	char fmstr[FMTSTRSIZE];
	fmstr[0] = '\0';
	while(1)
	{
		c = getopt(argc, argv, "-H:MSy:md");
		if(c < 0)
		{
			break;
		}
		switch(c)
		{
			case 1:
				fp = fopen(argv[optind-1],"w");
				if(fp == NULL)
				{	
					perror("fopen()");
					fp = stdout;
				}	
				break;
			case 'H':
				if(strcmp(optarg, "12") == 0)
					strncat(fmstr, "%I(%p) ", FMTSTRSIZE - strlen(fmstr) - 1);
				else if(strcmp(optarg, "24") == 0)
					strncat(fmstr, "%H ", FMTSTRSIZE - strlen(fmstr) - 1);
				break;
			case 'M':
				strncat(fmstr, "%M ",FMTSTRSIZE - strlen(fmstr)-1);
				break;
			case 'S':
				strncat(fmstr, "%S ", FMTSTRSIZE - strlen(fmstr) - 1);
				break;
			case 'y':
				if(strcmp(optarg, "2") == 0)
					strncat(fmstr, "%y ", FMTSTRSIZE - strlen(fmstr) - 1);
				else if(strcmp(optarg, "4")==0)
					strncat(fmstr, "%Y ", FMTSTRSIZE - strlen(fmstr) -1);
				break;
			case 'm':
				strncat(fmstr, "%m ",FMTSTRSIZE-strlen(fmstr) - 1);
				break;
			case 'd':
				strncat(fmstr, "%d ", FMTSTRSIZE-strlen(fmstr)-1);
				break;
			default:
				break;
		}
	}
	stamp = time(NULL);
	tm = localtime(&stamp);

	strncat (fmstr, "\n", FMTSTRSIZE - strlen(fmstr) -1);
	strftime(timestr,TIMESTRSIZE, fmstr, tm);
	fputs(timestr, fp);

	if(fp != stdout)
		fclose(fp);
	exit(0);
}
