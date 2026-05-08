#ifndef __RELAYER_H
#define __RELAYER_H

#define REL_JOBMAX	10000  //最大工作数组容量
enum
{
	STATE_RUNNING = 1,
	STATE_CANCELED,
	STATE_OVER
};
struct rel_stat_st
{
	int state;
	int fd1;
	int fd2;
	int64_t count12, count21;
//	struct timerval start, end;

};

int rel_addjob(int fd1, int fd2);
/*
 *return >= 0       成功，返回当前任务id
 *      == -EINVAL  失败，参数非法
 *      == -ENOSPC  失败，空间不够
 *      == -ENOMEM  失败，内存分配有误
 * */

int rel_canceljob(int id);
/*return == 0		成功，指定任务取消
 *       == -EINVAL	失败，id不合法
 *       == -EBUSY	失败，重复取消一个任务
 *       
 * */
 
int  rel_waitjob(int id,struct rel_stat_st* );
/*
 * return == 0 		成功，指定任务已终止并回填状态
 *	  == -EINVAL    失败，参数非法
 *	  
 * */

int rel_statjob(int id, struct rel_stat_st*);
/*
 * return == 0 		成功，指定任务状态回填
 *        == -EINVAL    失败，参数非法
 * */





#endif
