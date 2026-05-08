#ifndef __PROTO_H
#define __PROTO_H

#include <stdint.h> 
#define NAMESIZE    11 //故意使用不能对齐的数值
#define RCVPORT     "1988"    //接收端口(为了避免使用没有类型的字面量，所以使用字符串型，随后使用atoi进行转换)
#define SNDPORT     "1989"
#define MTGROUP     "ff02::1"  //多播组组号,只能在本地链路上组播。如果要跨路由器，需要使用其他范围的组播地址（如 ff0e::1）

struct msg_st
{
    uint8_t name[NAMESIZE];
    uint32_t math;
    uint32_t chinese;
}__attribute__((packed));//告诉编译器不进行对齐


#endif