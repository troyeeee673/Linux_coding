#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "anytimer.h"


static void f1(void *arg)
{
    printf("f1: %s\n", (char *)arg);
}

static void f2(void *arg)
{
    printf("f2: %s\n", (char *)arg);
}

int main()
{
	int job1, job2, job3;
	puts("Begin!");
	job1 = at_addjob_repeat(5, f1, "aaa");
	if(job1 < 0)
	{
		fprintf(stderr, "at_addjob():%s\n", strerror(-job1));
		exit(1);
	}
	job2 = at_addjob(2, f2, "bbb");
	if(job2 < 0)
        {
                fprintf(stderr, "at_addjob():%s\n", strerror(-job2));
                exit(1);
        }
	job3 = at_addjob(7, f1, "ccc");
	if(job3 < 0)
        {
                fprintf(stderr, "at_addjob():%s\n", strerror(-job3));
                exit(1);
        }
	puts("End!");
	while(1)
	{
		write(1, ".", 1);
		sleep(1);
	}
	exit(0);
}
