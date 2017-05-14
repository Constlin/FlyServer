/********************************
operation about a buffer, which is 
used for read/write socket

Author: Andrew lin
********************************/
#ifndef _FLY_BUF_H
#define _FLY_BUF_H

//todo: make this value more suitable
#define CONNECTION_READ_BUFFER 1024
//todo: make this value more suitable
#define CONNECTION_WRITE_BUFFER 1024

struct fly_buf {
	//the begin of a buffer
    void *start;
    //the end of a buffer
    void *end;
    //the length of a buffer
    int   length;    
};

typedef struct fly_buf fly_buf_t;

//get a buffer of bytes and return
fly_buf_t *fly_init_buf(int bytes);

int fly_free_buf(fly_buf_t *buf)

#endif