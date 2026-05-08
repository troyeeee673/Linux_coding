#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
void alrm_handler()
{
	alarm(1);
	printf("SIGINT CATHCED\n");

}
int main()
{
	signal(SIGALRM, alrm_handler);
	alarm(1);
	while(1)
	{
		printf("while(1)\n");
		pause();
	}
	return 0;
}
