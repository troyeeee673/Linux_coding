#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
static volatile int loop = 1;
static void alm_handler(int s)
{
	loop = 0;
}
int main()
{
	int64_t count = 0;
	alarm(5);
	signal(SIGALRM, alm_handler);
	while(loop)
		count++;
	printf("%ld\n", count);
	exit(0);
}
