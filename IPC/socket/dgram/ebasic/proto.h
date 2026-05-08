#ifndef __PROTO_H
#define __PROTO_H

#include <stdint.h> 

//修改为使用变长结构体进行数据传输
#define NAMEMAX    (512 - 8 - 8) //512是推荐数据包大小，减去8（成绩占据大小），再减8（UDP包头大小）
#define RCVPORT     "1988"    //接收端口(为了避免使用没有类型的字面量，所以使用字符串型，随后使用atoi进行转换)
#define SNDPORT     "1989"

struct msg_st
{
    uint32_t math;
    uint32_t chinese;
    uint8_t name[1];
}__attribute__((packed));//告诉编译器不进行对齐


#endif