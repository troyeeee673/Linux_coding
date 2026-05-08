#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "anytimer.h"

//声明数据结构
enum
{
    STATE_RUNNING = 1,
    STATE_CANCELED,
    STATE_OVER

};

struct at_job_st
{
    int job_state;
    int sec;//持续时间
    int time_remain;
    at_jobfunc_t *jobp;
    void* arg;//参数
    int repeat;//是否是周期性任务
};

static struct at_job_st * job[JOB_MAX];
static int initied = 0;
static struct sigaction sa_save;

static int get_free_pos(void)
{
    int i;
    for(i = 0 ;i < JOB_MAX;i++)
    {
        if(job[i] == NULL)
            return i;
        
    }
    return -1;
}


static void alrm_action(int s, siginfo_t * infop, void * unused)
{
    if(infop -> si_code != SI_KERNEL)
        return;
    for(int i = 0 ;i < JOB_MAX ;i ++)
    {
        if(job[i] != NULL && job[i] -> job_state == STATE_RUNNING)
        {
            job[i]->time_remain--;
            if(job[i]->time_remain == 0)
            {
                job[i]->jobp(job[i]->arg);
                if(job[i]->repeat == 1)
                {
                    job[i]->time_remain = job[i]->sec;
                }
                else job[i]->job_state = STATE_OVER;
            }
        }
    }

}
static void module_unload()
{
    struct itimerval itv;

    //关闭时钟
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &itv, NULL) < 0)
    {
        perror("setitimer()");
        exit(1);
    }

    //恢复信号行为
    if(sigaction(SIGALRM, &sa_save, NULL) < 0)
    {
        perror("sigaction()");
        exit(1);
    }
}

static void module_load()
{
    struct sigaction sa;
    struct itimerval itv;

    //信号行为注册
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = alrm_action;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM, &sa, &sa_save) < 0)
    {
        perror("sigaction()");
        exit(1);
    }

    //第一个时钟信号发出
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &itv, NULL) < 0)
    {
        perror("setitimer()");
        exit(1);
    }

    atexit(module_unload);
}

//添加任务
int at_addjob(int sec, at_jobfunc_t *jobp, void * arg)
{
    int pos ;
    struct at_job_st* me;

    if(sec < 0)
    {
        return -EINVAL;
    }
    if(!initied)
    {
        module_load();
        initied = 1;
    }

    pos = get_free_pos();
    if(pos < 0)
    {
        return -ENOSPC;//没有空位
    }
    me = malloc(sizeof(*me));
    if(me == NULL)
        return -ENOMEM;//没有内存
    
    me->job_state = STATE_RUNNING;
    me->sec = sec;
    me->time_remain = sec;
    me->jobp = jobp;
    me->arg = arg;
    me->repeat = 0;

    job[pos] = me;
    return pos;
}

int at_addjob_repeat(int sec, at_jobfunc_t * jobp, void* arg)
{
    int pos ;
    struct at_job_st* me;

    if(sec < 0)
    {
        return -EINVAL;
    }
    if(!initied)
    {
        module_load();
        initied = 1;
    }

    pos = get_free_pos();
    if(pos < 0)
    {
        return -ENOSPC;//没有空位
    }
    me = malloc(sizeof(*me));
    if(me == NULL)
        return -ENOMEM;//没有内存

    me->arg = arg;
    me->time_remain = sec;
    me->jobp = jobp;
    me->repeat = 1;
    me->job_state = STATE_RUNNING;
    me->sec = sec;

    job[pos] = me;
    return pos;

}

//取消任务
int at_canceljob(int id)
{
    if(id < 0 || id >= JOB_MAX || job[id] == NULL)
        return -EINVAL;// 参数不合法
    if(job[id]->job_state == STATE_CANCELED)
        return -ECANCELED;
    if(job[id]->job_state == STATE_OVER)
        return -EBUSY;
    job[id]->job_state = STATE_CANCELED;
    return 0;
}

//等待任务，收尸
int at_waitjob(int id)
{
    if(id < 0 || id >= JOB_MAX || job[id] == NULL)
        return -EINVAL;
    if(job[id]->repeat == 1)
        return -EBUSY;
    while(job[id]->job_state == STATE_RUNNING)
        pause();
    if(job[id]->job_state == STATE_CANCELED || job[id]->job_state == STATE_OVER)
        {
            free(job[id]);
            job[id] = NULL;
        }

    return 0;
}