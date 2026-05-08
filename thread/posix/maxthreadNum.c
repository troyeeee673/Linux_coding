#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

void *func(void * p)
{
	int i = 0 ;
	fprintf(stdout, "%p\n", &i);
}

int main()
{
	int i , err;
	pthread_t tid[1000];
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	pthread_attr_setstacksize(&attr, 1024*1024*2);
	for(i = 0 ;i < 1000;i++)
	{
		//err = pthread_create(tid + i, NULL, func, NULL);
		err = pthread_create(tid + i, &attr, func, NULL);
		if(err)
		{
			fprintf(stderr,"pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}

	for(i = 0 ;i < 1000 ;i++)
	{
		pthread_join(tid[i], NULL);
	}

	pthread_attr_destroy(&attr);
	exit(0);
}
