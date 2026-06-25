#define _DEFAULT_SOURCE
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

#define THREAD_NUM 12
#define QUEUE_MAX 1024

// ==================== 全局统计（互斥锁保护） ====================
static long file_count = 0, code_count = 0, dir_count = 0;
static pthread_mutex_t stat_mutex = PTHREAD_MUTEX_INITIALIZER;

// ==================== 任务队列（互斥锁 + 条件变量） ====================
static char *task_queue[QUEUE_MAX];
static int queue_front = 0;  // 队头（取任务）
static int queue_rear = 0;   // 队尾（放任务）
static int queue_count = 0;  // 当前队列中任务数
static int done_flag = 0;    // 是否所有任务已完成（不再有新任务）

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;  // 队列非空
static pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;   // 队列非满

// ==================== 工具函数 ====================

// 统计单个文件的代码行数
long code_counter(char *filepath)
{
    int ch;
    long cnt = 0;
    FILE *fp = fopen(filepath, "r");
    if (!fp)
    {
        fprintf(stderr, "file:%s\n", filepath);
        perror("fopen() in code_counter");
        return 0;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
            cnt++;
    }
    cnt++;
    fclose(fp);
    return cnt;
}

// 往任务队列放入一个目录路径（调用者需确保 path 是堆内存，且本函数会接管所有权）
void enqueue_task(char *path)
{
    pthread_mutex_lock(&queue_mutex);

    // 如果队列满了，等待
    while (queue_count >= QUEUE_MAX && !done_flag)
    {
        pthread_cond_wait(&cond_not_full, &queue_mutex);
    }

    if (queue_count < QUEUE_MAX)
    {
        task_queue[queue_rear] = path;
        queue_rear = (queue_rear + 1) % QUEUE_MAX;
        queue_count++;

        // 唤醒一个等待的工作线程
        pthread_cond_signal(&cond_not_empty);
    }
    else
    {
        // 队列满且 done，释放 path 避免泄漏
        free(path);
    }

    pthread_mutex_unlock(&queue_mutex);
}

// 从任务队列取出一个目录路径（返回的指针需要调用者 free）
char *dequeue_task()
{
    pthread_mutex_lock(&queue_mutex);

    // 队列为空且没结束，等待
    while (queue_count == 0 && !done_flag)
    {
        pthread_cond_wait(&cond_not_empty, &queue_mutex);
    }

    char *path = NULL;
    if (queue_count > 0)
    {
        path = task_queue[queue_front];
        queue_front = (queue_front + 1) % QUEUE_MAX;
        queue_count--;

        // 唤醒可能等待的生产者
        pthread_cond_signal(&cond_not_full);
    }

    pthread_mutex_unlock(&queue_mutex);
    return path;
}

// 通知所有线程：不再有新任务
void set_done()
{
    pthread_mutex_lock(&queue_mutex);
    done_flag = 1;
    // 广播唤醒所有等待的线程，让它们检查 done_flag 后退出
    pthread_cond_broadcast(&cond_not_empty);
    pthread_mutex_unlock(&queue_mutex);
}

// ==================== 工作线程函数 ====================

void *worker(void *arg)
{
    while (1)
    {
        char *dir_name = dequeue_task();
        if (dir_name == NULL)
        {
            // done_flag 为真且队列为空，退出线程
            break;
        }

        // ============ 开始遍历这个目录 ============
        DIR *dir = opendir(dir_name);
        if (!dir)
        {
            fprintf(stderr, "opendir(): %s\n", dir_name);
            free(dir_name);
            continue;  // 继续取下一个任务
        }

        printf("\n[线程 %lu] 目录: %s\n", pthread_self(), dir_name);

        struct dirent *dirent;
        errno = 0;

        while ((dirent = readdir(dir)) != NULL)
        {
            if (strcmp(dirent->d_name, ".") == 0 ||
                strcmp(dirent->d_name, "..") == 0)
                continue;

            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s",
                     dir_name, dirent->d_name);

            struct stat st;
            if (stat(full_path, &st) == -1)
            {
                perror("stat");
                continue;
            }

            if (S_ISREG(st.st_mode))  // 普通文件
            {
                // 更新统计（需要加锁）
                pthread_mutex_lock(&stat_mutex);
                file_count++;
                pthread_mutex_unlock(&stat_mutex);

                size_t len = strlen(full_path);
                long lines = 0;
                if (len > 2 && full_path[len - 2] == '.' && 
                    (full_path[len - 1] == 'c' || full_path[len - 1] == 'h'))
                {
                    lines = code_counter(full_path);
                }

                if (lines > 0)
                {
                    pthread_mutex_lock(&stat_mutex);
                    code_count += lines;
                    pthread_mutex_unlock(&stat_mutex);
                }

                printf("- %ld\t%s\n", st.st_ino, dirent->d_name);
            }
            else if (S_ISDIR(st.st_mode))  // 子目录 → 放入任务队列
            {
                pthread_mutex_lock(&stat_mutex);
                dir_count++;
                pthread_mutex_unlock(&stat_mutex);

                char *path_copy = strdup(full_path);
                if (path_copy)
                {
                    enqueue_task(path_copy);  // 放入队列，其他线程会处理
                }
                printf("d %ld\t%s\n", st.st_ino, dirent->d_name);
            }
            else if (S_ISLNK(st.st_mode))
            {
                printf("l %ld\t%s\n", st.st_ino, dirent->d_name);
            }

            errno = 0;
        }

        if (errno != 0)
        {
            perror("readdir()");
        }

        closedir(dir);
        free(dir_name);  // 释放 dequeue_task 返回的路径
    }

    return NULL;
}

// ==================== 主函数 ====================

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "用法: %s <目录>\n", argv[0]);
        exit(1);
    }

    pthread_t tids[THREAD_NUM];

    // 1. 创建线程池
    for (int i = 0; i < THREAD_NUM; i++)
    {
        pthread_create(&tids[i], NULL, worker, NULL);
    }

    // 2. 把起始目录放入任务队列
    char *start_path = strdup(argv[1]);
    if (!start_path)
    {
        perror("strdup");
        exit(1);
    }
    enqueue_task(start_path);

    // 3. 等待所有任务完成（简单做法：等任务队列为空后，设置 done）
    //    更稳妥的做法是：用一个原子计数器跟踪"活跃任务数"，
    //    但这里用最简单的方式：主线程等待队列清空后 done。
    while (1)
    {
        pthread_mutex_lock(&queue_mutex);
        int empty = (queue_count == 0);
        pthread_mutex_unlock(&queue_mutex);

        if (empty)
        {
            // 队列空了，但可能有线程正在处理最后一个目录，
            // 还会产生新的子目录任务。简单 sleep 一下再检查。
            usleep(100000);  // 100ms
            pthread_mutex_lock(&queue_mutex);
            int still_empty = (queue_count == 0);
            pthread_mutex_unlock(&queue_mutex);
            if (still_empty)
                break;
        }
        else
        {
            usleep(50000);  // 50ms
        }
    }

    // 4. 通知所有线程结束
    set_done();

    // 5. 等待所有线程退出
    for (int i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(tids[i], NULL);
    }

    // 6. 打印最终结果
    printf("\n===== 统计结果 =====\n");
    printf("文件数：%ld\n", file_count);
    printf("目录数：%ld\n", dir_count);
    printf("代码行数：%ld\n", code_count);

    return 0;
}