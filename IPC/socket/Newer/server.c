#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>

#include <sys/un.h> //用于本地通信
#include <netinet/in.h>

#define THREAD_MAX 4

typedef struct
{
    void (*function)(void *);
    void *arg;
} task_t;

typedef struct task_node
{
    task_t task;
    struct task_node *next;
} task_node_t;

typedef struct
{
    task_node_t *head;
    task_node_t *tail;
    int count;
} task_queue_t;

typedef struct
{
    pthread_t threads[THREAD_MAX];
    task_node_t *task_head;
    task_node_t *task_tail;
    int task_count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int shutdown;
} thread_pool_t;

// 全局线程池
thread_pool_t pool;

// 初始化线程池
void pool_init()
{
    pool.task_head = NULL;
    pool.task_tail = NULL;
    pool.task_count = 0;
    pool.shutdown = 0;
    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.cond, NULL);
}

// 添加任务到队列
void pool_add_task(void (*func)(void *), void *arg)
{
    task_node_t *node = (task_node_t *)malloc(sizeof(task_node_t));
    node->task.function = func;
    node->task.arg = arg;
    node->next = NULL;

    pthread_mutex_lock(&pool.mutex);

    // 添加到队列尾部
    if (pool.task_tail == NULL)
    {
        pool.task_head = node;
        pool.task_tail = node;
    }
    else
    {
        pool.task_tail->next = node;
        pool.task_tail = node;
    }
    pool.task_count++;

    pthread_cond_signal(&pool.cond);
    pthread_mutex_unlock(&pool.mutex);
}

// 工作线程请求
void *worker_thread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&pool.mutex);

        // 等待任务
        while (pool.task_count == 0 && !pool.shutdown)
        {
            pthread_cond_wait(&pool.cond, &pool.mutex);
        }

        // 检查是否退出
        if (pool.shutdown)
        {
            pthread_mutex_unlock(&pool.mutex);
            pthread_exit(NULL);
        }

        // 取任务
        task_node_t *node = pool.task_head;
        pool.task_head = node->next;
        if (pool.task_head == NULL)
        {
            pool.task_tail = NULL;
        }
        pool.task_count--;
        pthread_mutex_unlock(&pool.mutex);
        // 执行任务
        node->task.function(node->task.arg);
        free(node);
    }
    return NULL;
}

// 处理客户端请求
void handle_client(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);

    char buf[1024];
    int client_active = 1;
    while (client_active)
    {
        memset(buf, 0, sizeof(buf));
        int size = read(client_fd, buf, sizeof(buf) - 1);
        if (size <= 0)
        {
            printf("客户端断开连接\n");
            break;
        }

        buf[strcspn(buf, "\r\n")] = '\0';
        printf("收到：%s\n", buf);

        if (strcmp(buf, "exit") == 0)
        {
            printf("客户端取消连接\n");
            break;
        }
        else
        {
            char message[] = "This is a message from server.\n";
            write(client_fd, message, sizeof(message));
        }
    }
    close(client_fd);
}
int main()
{
    // 1. 创建服务端套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 设置端口复用
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. 绑定地址
    // sturct sockaddr_un addr;用于本地通信
    struct sockaddr_in addr;        // 用于网络通信
    memset(&addr, 0, sizeof(addr)); // 初始化addr空间为0
    addr.sin_family = AF_INET;
    // 端口号，从1024往后使用
    addr.sin_port = htons(9000);
    // 地址本质是一个无符号32位整数
    // 这里的地址写INADDR_ANY，相当于0.0.0.0,表示服务器可以接收任何ip的请求
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    // 3. 监听
    listen(fd, 9); // 9是最大请求个数
    printf("服务器准备接受请求\n");

    // 初始化线程池
    pool_init();
    
    for (int i = 0; i < THREAD_MAX; i++)
    {
        pthread_create(&pool.threads[i], NULL, worker_thread, NULL);
    }

    // 4. 接收请求（接收多个）
    while (1)
    {
        // 用于接收client数据的
        struct sockaddr_in addr_client;
        memset(&addr_client, 0, sizeof(addr_client));
        // 接受请求
        printf("服务器等待连接\n");
        // 建立一个连接
        // 必须单独定义一个socklen_t变量存长度
        socklen_t addr_len = sizeof(addr_client);
        int fd_client = accept(fd, (struct sockaddr *)&addr_client, &addr_len);

        int *client_fd = (int*)malloc(sizeof(int));
        *client_fd = fd_client;

        //将客户端处理任务加入线程池
        pool_add_task(handle_client, client_fd);
        printf("新客户端建立连接， fd = %d\n", fd_client);
    }
    close(fd);
    return 0;
}