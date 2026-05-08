#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "relayer.h"

#define BUFSIZE 1024

enum
{
    STATE_R = 1,
    STATE_W,
    STATE_EX,
    STATE_T
};

// 定义状态机
struct rel_fsm_st
{
    int state; // ❗ 这里原来缺少分号，导致编译器报错
    int sfd;   // 读取源
    int dfd;   // 写目标
    char buf[BUFSIZE];
    int len;
    const char *strerr; // 改为 const char*
    int pos;
    int64_t count;
};

// 定义job
struct rel_job_st
{
    int job_state; // 状态在头文件
    int fd1;
    int fd2;
    struct rel_fsm_st fsm12, fsm21;
    int fd1_save, fd2_save;
    //	struct timerval start, end;
};

static struct rel_job_st *rel_job[REL_JOBMAX];
static pthread_mutex_t mut_rel_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

static void fsm_driver(struct rel_fsm_st *fsm)
{
    int ret;
    switch (fsm->state)
    {
    case STATE_R:
        fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE); // ❗ 原来是 buf，改成 fsm->buf
        if (fsm->len == 0)
        {
            fsm->state = STATE_T;
        }
        else if (fsm->len < 0)
        {
            if (errno == EAGAIN)
            {
                fsm->state = STATE_R;
            }
            else
            {
                fsm->strerr = "read()"; // ❗ 直接赋值，不要用 strcpy
                fsm->state = STATE_EX;
            }
        }
        else
        {
            fsm->pos = 0;
            fsm->state = STATE_W;
        }
        break;
    case STATE_W:
        ret = write(fsm->dfd, fsm->buf + fsm->pos, fsm->len);
        if (ret < 0)
        {
            if (errno == EAGAIN)
                fsm->state = STATE_W; // ❗ 写阻塞应该保持 STATE_W，等下次继续写
            else
            {
                fsm->strerr = "write()";
                fsm->state = STATE_EX;
            }
        }
        else
        {
            fsm->pos += ret;
            fsm->len -= ret;
            if (fsm->len == 0)
                fsm->state = STATE_R;
            else
                fsm->state = STATE_W;
        }
        break;
    case STATE_EX:
        perror(fsm->strerr);
        fsm->state = STATE_T;
        break;
    case STATE_T:
        /* do sth */
        break;
    default:
        abort(); // ❗ 原来是 abo r t()
        break;
    }
}



static void *thr_relayer(void *)
{
    while (1)
    {
        pthread_mutex_lock(&mut_rel_job);
        for (int i = 0; i < REL_JOBMAX; i++)
        {
            if (rel_job[i] != NULL)
            {
                if(rel_job[i]->job_state == STATE_RUNNING)
                {
                    fsm_driver(&rel_job[i]->fsm12);
                    fsm_driver(&rel_job[i]->fsm21);
                    if(rel_job[i]->fsm12.state == STATE_T && rel_job[i]->fsm21.state == STATE_T)
                        rel_job[i]->job_state = STATE_OVER;
                    
                }
            }
        }
        pthread_mutex_unlock(&mut_rel_job);
    }
}

static void moudule_unload(void){}

static void moudule_load(void)
{
    pthread_t tid_relayer;
    int err;
    err = pthread_create(&tid_relayer, NULL, thr_relayer, NULL);
    if (err)
    {
        fprintf(stderr, "pthread_create():%s\n", strerror(err));
        exit(1);
    }
}

static int get_free_pos_unlocked(void)
{
    // 需要实现：查找空闲的 job 槽位
    for (int i = 0; i < REL_JOBMAX; i++) {
        if (rel_job[i] == NULL)
            return i;
    }
    return -1;
}

int rel_addjob(int fd1, int fd2)
{
    struct rel_job_st *me;
    int pos;

    pthread_once(&init_once, moudule_load); // 动态模块的单次调用

    me = malloc(sizeof(*me));
    if (me == NULL)
    {
        return -ENOMEM;
    }
    me->fd1 = fd1;
    me->fd2 = fd2;
    me->job_state = STATE_RUNNING;

    me->fd1_save = fcntl(me->fd1, F_GETFL);
    fcntl(me->fd1, F_SETFL, me->fd1_save | O_NONBLOCK);
    me->fd2_save = fcntl(me->fd2, F_GETFL);
    fcntl(me->fd2, F_SETFL, me->fd2_save | O_NONBLOCK);

    me->fsm12.state = STATE_R;
    me->fsm12.sfd = fd1;
    me->fsm12.dfd = fd2;

    me->fsm21.state = STATE_R;
    me->fsm21.sfd = fd2;
    me->fsm21.dfd = fd1;

    pthread_mutex_lock(&mut_rel_job);
    pos = get_free_pos_unlocked();
    if (pos < 0)
    {
        // 先对内容进行恢复
        pthread_mutex_unlock(&mut_rel_job); // 防止死锁
        fcntl(me->fd1, F_SETFL, me->fd1_save);
        fcntl(me->fd2, F_SETFL, me->fd2_save);
        free(me); // 释放已分配的内存
        return -ENOSPC;
    }

    rel_job[pos] = me;

    pthread_mutex_unlock(&mut_rel_job);
}

/*
int rel_canceljob(int id);


int  rel_waitjob(int id,struct rel_stat_st* );


int rel_statjob(int id, struct rel_stat_st*);
*/