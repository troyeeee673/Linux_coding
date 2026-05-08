#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	alarm(10);
	alarm(1);
	alarm(5);//最后这次会覆盖前面的
	while(1)
		pause();
	exit(0);
}
