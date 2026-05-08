#ifndef __MYPIPE_H
#define __MYPIPE_H

#include <stddef.h>
#define PIPESIZE    1024
#define __MYPIPE_READ   0x00000001UL
#define __MYPIPE_WRITE  0x00000002UL

typedef void mypipe_t;

mypipe_t* mypipe_init(void);

int mypipe_register(mypipe_t* , int opmap);//使用位图指定身份

int mypipe_unregister(mypipe_t *, int opmap);

int mypipe_read(mypipe_t*, void* buf, size_t count);


int mypipe_write(mypipe_t*, const void* buf, size_t);

int mypipe_destroy(mypipe_t*);


#endif