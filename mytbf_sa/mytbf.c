#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include "mytbf.h"



static struct mytbf_st * job[MYTBF_MAX];
static int inited = 0;
//static sighandler_t alrm_handler_save;
static struct sigaction alrm_sa_save;
struct mytbf_st
{
	int cps;
	int burst;
	int token;
	int pos;
};

static int get_free_pos()
{
	for(int i = 0;i<MYTBF_MAX;i++)
	{
		if(job[i] == NULL)
			return i;
	}
	return -1;
}
static void alrm_action(int s, siginfo_t *infop, void *unused)
{
//	alarm(1);

	if(infop->si_code != SI_KERNEL)
		return ;
	for(int i = 0;i<MYTBF_MAX;i++)
	{
		if(job[i] != NULL)
		{
			job[i]->token += job[i] ->cps;
			if(job[i]->token > job[i]->burst)
			{
				job[i]->token = job[i]->burst;
			}
		}
	}
}

static void module_unload()
{
        //signal(SIGALRM, alrm_handler_save);
        //alarm(0);
	struct itimerval itv;
	sigaction(SIGALRM, &alrm_sa_save, NULL);

	itv.it_interval.tv_sec = 1; // 定时器循环周期
        itv.it_interval.tv_usec = 0;
        itv.it_value.tv_sec = 1;////定时器首次触发时间
        itv.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &itv, NULL);
        for(int i = 0;i< MYTBF_MAX;i++)
                free(job[i]);
}

static void module_load()
{
	//alrm_handler_save = signal(SIGALRM,alrm_handler );
	//alarm(1);
	struct sigaction sa;
	struct itimerval itv;
	sa.sa_sigaction = alrm_action;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGALRM, &sa, &alrm_sa_save);

	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv,NULL );
	atexit(module_unload);
}


mytbf_t*  mytbf_init(int cps, int burst)
{
	struct mytbf_st *me;
	int pos;
	if(!inited)
	{
		module_load();
		inited = 1;
	}
	
	pos = get_free_pos();
	if(pos == -1)
	{
		return NULL;
	}
	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	me->pos = pos;
	job[pos] = me;

	return me;
}

static int min(int a, int b)
{
	return a < b ? a:b;
}
int mytbf_fetchToken(mytbf_t* ptr, int size)
{
	struct mytbf_st * me = ptr;
	if(size <=0)
		return -EINVAL;
	while(me -> token <=0)
		sigsuspend(&oldmask);
	int n = min(me -> token, size);
	me -> token -= n;
	return n;

}

int mytbf_returnToken(mytbf_t *ptr, int size)
{
	struct mytbf_st* me = ptr;
	if(size <=0)
		return -EINVAL;
	me->token += size;
	if(me->token > me -> burst)
		me->token = me->burst;
	return  size;
}
int mytbf_destroy(mytbf_t* ptr)
{
	struct mytbf_st *me = ptr;
	job[me -> pos] = NULL;
	free(ptr);
	return 0;
}
