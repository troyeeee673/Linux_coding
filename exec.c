#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
	puts("Bgein");
	fflush(NULL);//一定要在使用exec族函数前刷新所有流
	execl("/bin/date", "date", "+%s", NULL);
	perror("exec()");
	exit(1);

	puts("End");
	exit(0);
}
