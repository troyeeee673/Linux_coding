//制定通信协议

#ifndef __PROTO_H
#define __PROTO_H

#define KEYPATH     "/etc/services"
#define KEYPROJ     'g'
#define NAMESIZE    10

//接受双方使用如下格式进行数据发送和解析
struct msg_st
{
    long mtype;
    char name[NAMESIZE];
    int math;
    int chinese;
};


#endif