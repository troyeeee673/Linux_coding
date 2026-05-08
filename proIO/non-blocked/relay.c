#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define TTY1 "/dev/pts/1"
#define TTY2 "/dev/pts/2"
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
    int state;           // ❗ 这里原来缺少分号，导致编译器报错
    int sfd;             // 读取源
    int dfd;             // 写目标
    char buf[BUFSIZE];
    int len;
    const char* strerr;  // 改为 const char*
    int pos;
};

static void fsm_driver(struct rel_fsm_st *fsm)
{
    int ret;
    switch(fsm->state)
    {
        case STATE_R:
            fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);  // ❗ 原来是 buf，改成 fsm->buf
            if(fsm->len == 0)
            {
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
                    fsm->strerr = "read()";  // ❗ 直接赋值，不要用 strcpy
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
                    fsm->state = STATE_W;  // ❗ 写阻塞应该保持 STATE_W，等下次继续写
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
            /* do sth */
            break;
        default:
            abort();  // ❗ 原来是 abo r t()
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
    fd1 = open(TTY1, O_RDWR);
    if(fd1 < 0)
    {
        perror("open()");
        exit(1);
    }
	write(fd1, "TTY1\n", 5);

    fd2 = open(TTY2, O_RDWR | O_NONBLOCK);
    if(fd2 < 0)
    {
        perror("open()");
        exit(1);
    }
	write(fd2, "TTY2\n", 5);

    relay(fd1, fd2);
    close(fd1);
    close(fd2);
    exit(0);
}