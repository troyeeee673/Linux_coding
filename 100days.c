#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TIMESTRSIZE	1024
int main()
{

	time_t stamp;
	struct tm * tm;
	char timestr[TIMESTRSIZE];
	stamp = time(NULL);
	tm = localtime(&stamp);
	strftime(timestr,TIMESTRSIZE, "Now:%Y-%m-%d", tm);
	putsq(timestr);


	stamp = mktime(tm) + 100 * 24 * 60 * 60;
	tm = localtime(&stamp);
	strftime(timestr, TIMESTRSIZE,"100days Later:%Y-%m-%d", tm);
	puts(timestr);




	exit(0);
}
