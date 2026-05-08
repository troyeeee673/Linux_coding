#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main()
{
	time_t end;
	int64_t count = 0;
	end = time(NULL) + 5;

	while(time(NULL) <= end)
	{
		count ++;
	}
	printf("%ld\n",count); 
	exit(0);
}
