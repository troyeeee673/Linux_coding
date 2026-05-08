#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include "proto.h"

int main()
{
    key_t key;
    int msgid;
    struct msg_st rbuf;
    key = ftok(KEYPATH, KEYPROJ);
    if(key < 0)
    {
        perror("ftok()");
        exit(1);
    }

    msgid = msgget(key, IPC_CREAT | 0600);//创建新的消息队列
    if(msgid < 0)
    {
        perror("msgget()");
        exit(1);
    }

    while(1)
    {
        if(msgrcv(msgid, &rbuf, sizeof(rbuf) - sizeof(long), 0, 0) < 0)
        {
            perror("msgrcv()");
            exit(1);
        }
        printf("NAME = %s\n", rbuf.name);
        printf("Math = %d\n", rbuf.math);
        printf("Chinese = %d\n", rbuf.chinese);
        
    }

    //这条语句执行不到，因为现在只能从循环中异常结束
    msgctl(msgid, IPC_RMID, NULL);

    exit(0);
}