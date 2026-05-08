#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

// ✅ 修改为文件路径
#define FILE1 "/tmp/input.txt"
#define FILE2 "/tmp/output.txt"
#define BUFSIZE 1024

enum
{
    STATE_R = 1,
    STATE_W,
    STATE_EX,
    STATE_T
};

struct rel_fsm_st
{
    int state;
    int sfd;
    int dfd;
    char buf[BUFSIZE];
    int len;
    const char* strerr;
    int pos;
};

static void fsm_driver(struct rel_fsm_st *fsm)
{
    int ret;
    switch(fsm->state)
    {
        case STATE_R:
            fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);
            if(fsm->len == 0)
            {
                // 文件读到末尾，进入终止状态
                fsm->state = STATE_T;
            }
            else if(fsm->len < 0)
            {
                if(errno == EAGAIN)
                {
                    fsm->state = STATE_R;
                }
                else
                {
                    fsm->strerr = "read()";
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
            if(ret < 0)
            {
                if(errno == EAGAIN)
                    fsm->state = STATE_W;
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
                if(fsm->len == 0)
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
            break;
        default:
            abort();
            break;
    }
}

static void relay(int fd1, int fd2)
{
    int fd1_save, fd2_save;
    struct rel_fsm_st fsm12, fsm21;

    fd1_save = fcntl(fd1, F_GETFL);
    fcntl(fd1, F_SETFL, fd1_save | O_NONBLOCK);

    fd2_save = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, fd2_save | O_NONBLOCK);

    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;

    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;

    while(fsm12.state != STATE_T || fsm21.state != STATE_T)
    {
        fsm_driver(&fsm12);
        fsm_driver(&fsm21);
    }

    fcntl(fd1, F_SETFL, fd1_save);
    fcntl(fd2, F_SETFL, fd2_save);
}

int main(int argc, char** argv)
{
    int fd1, fd2;
    
    // ✅ 修改打开模式：文件1需要存在，文件2创建或清空
    fd1 = open(FILE1, O_RDWR);
    if(fd1 < 0)
    {
        perror("open input file");
        exit(1);
    }

    fd2 = open(FILE2, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd2 < 0)
    {
        perror("open output file");
        close(fd1);
        exit(1);
    }

    relay(fd1, fd2);
    
    close(fd1);
    close(fd2);
    
    printf("File copy completed!\n");
    exit(0);
}