#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
#include "proto.h"

int main()
{
    key_t key;
    int msgid;
    struct msg_st sbuf;
    key = ftok(KEYPATH, KEYPROJ);
     if(key < 0)
    {
        perror("ftok()");
        exit(1);
    }

    msgid = msgget(key, 0);
    if(msgid < 0)
    {
        perror("msgget()");
        exit(1);
    }

    sbuf.mtype = 1;
    strcpy(sbuf.name, "Vicky");
    sbuf.math = 99;
    sbuf.chinese = 100;
    if(msgsnd(msgid, &sbuf, sizeof(sbuf) - sizeof(long), 0) < 0)
    {
        perror("msgrcv()");
        exit(1);
    
    }
    puts("OK");
    exit(0);
}