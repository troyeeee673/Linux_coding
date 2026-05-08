#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define LEFT    30000000
#define RIGHT   30000200
#define THRNUM	4

static int num = 0;
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;

static void* thr_primer(void * p);
int main()
{
	int i , k, err;
	pthread_t tid[THRNUM];


	for(i = 0;i < THRNUM;i++)
	{
		err = pthread_create(tid+i, NULL, thr_primer, (void *)i);
		if(err)
		{
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			for(k = 0 ; k < i; k++)
			{
				pthread_join(tid[k], NULL);
			}
			
			exit(1);
		}


	}

	for(i = LEFT; i <= RIGHT; i ++)
        {
                pthread_mutex_lock(&mut_num);
                while(num != 0)
                {
                        pthread_mutex_unlock(&mut_num);
                        sched_yield();//出让调度器给别的线程，可以
//理解为一个非常短暂的sleep,只不过不用从running->可终止的睡眠态,再转
//回来的过程
                        pthread_mutex_lock(&mut_num);
                }
                num = i;
                pthread_mutex_unlock(&mut_num);
        }


	pthread_mutex_lock(&mut_num);
        while(num != 0)
        {
                pthread_mutex_unlock(&mut_num);
                sched_yield();
                pthread_mutex_lock(&mut_num);
        }
        num = -1 ;
        pthread_mutex_unlock(&mut_num);


	for(i = 0; i < THRNUM;i++)
	{
		pthread_join(tid[i], NULL);
	}
		



	exit(0);
}

static void* thr_primer(void * p)
{
    
	int mark, i, j;
	while(1)
	{
	pthread_mutex_lock(&mut_num);
	while(num == 0)
	{
		pthread_mutex_unlock(&mut_num);
		sched_yield();
		pthread_mutex_lock(&mut_num);
	}
	if(num == -1)
	{
		pthread_mutex_unlock(&mut_num);//一定要记得在临界区中的跳转语句前加上解锁，否则会造成死锁
		break;
	}
        mark = 1, i = num;
	num = 0;
	pthread_mutex_unlock(&mut_num);

        for(j = 2;j <= i/2;j++)
        {
            if(i % j == 0)
            {
                mark = 0;
                break;
            }
        }
        if(mark)
            printf("[%d]%d is a primer\n", (int)p, i);
	}
    	pthread_exit(NULL);
}