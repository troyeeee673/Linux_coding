#ifndef __PROTOL2_H
#define __PROTOL2_H

#define KEYPATH     "/etc/services"
#define KEYPROJ     'g'
#define PATHMAX     1024
#define DATAMAX     1024


enum{
    MSG_PATH = 1,
    MSG_DATA, 
    MSG_EOT

};

//文件路径结构体，服务器端接收到的类型
typedef struct msg_path_st
{
    long mtype;//必须是MSG_PATH
    char path[PATHMAX];//带尾0的串

}msg_path_t;


//client端会接收到的包类型
//数据包结构体
typedef struct msg_s2c_st
{
    long mtype;//必须是MSG_DATA
    int datalen;//描述数据包有效数据长度
    /* 
    *   datalen > 0 :data包
    *   datalen = 0 :eot包
    * */
    char data[DATAMAX];
}msg_data_t;

//结束标志结构体
typedef  struct msg_eot_st
{
    long mtype;//必须是MSG_EOT
}msg_eot_t;

#endif