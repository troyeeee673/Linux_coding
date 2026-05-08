#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "mypipe.h"

struct mypipe_st{
    int head;
    int tail;
    char data[PIPESIZE];//存放字符串
    int datasize;//记录有效字符数
    int count_rd;
    int count_wr;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};


mypipe_t* mypipe_init(void)
{
    struct mypipe_st *me;
    
    me = malloc(sizeof(*me));
    if(me == NULL)
        return NULL;//不要进行perror报错，因为这里采用的是库函数的编程思想，要让用户根据返回值进行报错输出
        //再者，如果在库函数中大量使用报错输出，会和用户的报错输出产生混淆，并且，用户的使用情况是未知的，可能
        //用户并没有输出到标准输出，而是进行了重定向
    me->head = 0;
    me->tail = 0;
    me->count_rd = 0;
    me->count_wr = 0;
    me->datasize = 0;
    pthread_mutex_init(& me->mut, NULL);
    pthread_cond_init(&me->cond, NULL);
    
    return me;
}

static int next(int head)
{
    return head % PIPESIZE;
}

int mypipe_register(mypipe_t*ptr , int opmap)
{
    struct mypipe_st *me = ptr;
    if ((opmap & ~(__MYPIPE_READ | __MYPIPE_WRITE)) != 0)  // 有其他非法位
    return -1;
if (!(opmap & (__MYPIPE_READ | __MYPIPE_WRITE)))       // 没有选择任何一种
    return -1;
    
    pthread_mutex_lock(&me->mut);
    if(opmap & __MYPIPE_READ)
        me->count_rd ++;

    if(opmap & __MYPIPE_WRITE)
        me->count_wr ++;
    
    while (me->count_rd <=0 || me->count_wr <=0)
        pthread_cond_wait(&me->cond, &me->mut);
    
    pthread_cond_broadcast(&me->cond);//唤醒其他等待注册身份的线程
    //假如当前有十个线程要注册读者身份，但是没有写者，则大家都阻塞在while，一旦有一个
    //线程要注册写者身份，看到有10个读者，则通知读者线程，然后执行解锁
    
    pthread_mutex_unlock(&me->mut);
    return 0;
    

}

int mypipe_unregister(mypipe_t *ptr, int opmap)
{
    struct mypipe_st *me = ptr;
    if(opmap | 0x00000000 != 0x00000001 && opmap | 0x00000000 != 0x000000010)
        return -1;
    pthread_mutex_lock(&me->mut);

    if(opmap & __MYPIPE_READ)
        me->count_rd --;

    if(opmap & __MYPIPE_WRITE)
        me->count_wr --;
    
    pthread_cond_broadcast(&me->cond);
    
    pthread_mutex_unlock(&me->mut);
    return 0;

}

static int mypipe_readbyte_unlocked(struct mypipe_st *me, char* datap)
{
    if(me->datasize <=0)
        return -1;
    *datap = me->data[me->head];
    me->head = next(me->head);
    me->datasize --;
    return 0;
}

int mypipe_read(mypipe_t*ptr, void* buf, size_t count)
{
    int i ;
    struct mypipe_st *me = ptr;
    pthread_mutex_lock(&me->mut);

    while(me->datasize <=0 && me->count_wr > 0)//当没有数据且还存在写者时才等待
        pthread_cond_wait(&me->cond, &me->mut);
    
    //管道中没有数据且写者数量小于0，则解锁返回
    if(me->datasize <=0 && me->count_wr <= 0)
    {
        pthread_mutex_unlock(&me->mut);
        return 0;
    }
    for(i = 0 ;i< count;i++)
        if(mypipe_readbyte_unlocked(me, buf+i) != 0)
            break;
        
    pthread_cond_broadcast(&me->cond);//通知写者队列读出了一部分，有空位继续写
    pthread_mutex_unlock(&me->mut);
    return i;
    
}

int mypipe_writebyte_unlocked(struct mypipe_st *me, char* datap)
{
    if(me->datasize >=PIPESIZE)
        return -1;
    me->data[me->tail] = *datap;
    me->tail = next(me->tail);
    me->datasize ++;
    return 0;
}

int mypipe_write(mypipe_t*ptr, const void* buf, size_t count)
{
    int i ;
    struct mypipe_st *me = ptr;
    pthread_mutex_lock(&me->mut);

    while(me->datasize >=PIPESIZE && me->count_rd > 0)//当没有数据且还存在写者时才等待
        pthread_cond_wait(&me->cond, &me->mut);
    
    //管道中数据满且读者数量小于0，则解锁返回
    if(me->datasize >= PIPESIZE && me->count_rd <= 0)
    {
        pthread_mutex_unlock(&me->mut);
        return 0;
    }
    for(i = 0 ;i< count;i++)
        if(mypipe_writebyte_unlocked(me, buf+i) != 0)
            break;
        
    pthread_cond_broadcast(&me->cond);//通知写者队列读出了一部分，有空位继续写
    pthread_mutex_unlock(&me->mut);
    return i;

}


int mypipe_destroy(mypipe_t *ptr)
{
    struct mypipe_st *me = ptr;
    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);

    free(ptr);
    return 0;

}




