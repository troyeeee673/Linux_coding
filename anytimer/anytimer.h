#ifndef __ANYTIMER_H
#define __ANYTIMER_H

#define JOB_MAX     1024
typedef void at_jobfunc_t(void*);

//添加任务
int at_addjob(int sec, at_jobfunc_t *jobp, void * arg);

//取消任务
int at_canceljob(int id); 

//等待任务，收尸
int at_waitjob(int id);

//周期性任务添加
int at_addjob_repeat(int id, at_jobfunc_t * jobp, void* arg);
#endif


//使用一个时钟完成多任务的定时

