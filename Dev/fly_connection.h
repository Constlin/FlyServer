/********************************
operation about connection pool.

Author: Andrew lin
********************************/
#ifndef _FLY_CONNECTION_H
#define _FLY_CONNECTION_H

#include "fly_buf.h"

struct fly_listening {
    //the listening fd.
    int                 fd;

    //listen fd's sockaddr.
    struct sockaddr    *sockaddr;

    //sockaddr's length
    socketlen_t        *addrlen;

    //listener binding address
    char               *addr;

    //listener bingding port
    int                 port;

    //socket's family
    int                 family;

    //socket's type, sucha as SOCK_STREAM(for tcp connection)
    int                 type;

    //socket's protocol
    int                 proto;

    //the handler for the listener
    int               (*handler)(int fd, fly_process_t* proc);

    //the worker process which accept this listener.
    fly_process_t      *process;

};

typedef struct fly_listening fly_listening_t;

struct fly_connection {
	//the sockef fd that this fly_connection assicuated.
    int               fd;

    //the read event about this fly_connection.
    struct fly_event *read;

    //the write event about this fly_connection.
    struct fly_event *write;

    //the buffer used for read this connection's internet data.
    struct fly_buf_t *read_buf;

    //the buffer used for write data to this connection's internet.
    struct fly_buf_t *write_buf;

    //if the connection is used, set to 1, else 0
    int               used;

    //pointed to next free connection
    fly_connection_t *next_free;

    //the fly_listening_t that this connection associated
    fly_listening_t  *listener;

};

typedef struct fly_connection fly_connection_t;

fly_array_t *fly_connection_pool_init();

fly_connection_t *fly_get_connection(fly_process_t *proc);

//free a connection, notice we just make it reused insted of freeing it, so while get conn we need to memset(buf, 0, bufsize)
int fly_free_connection(fly_process_t *proc, fly_connection_t *conn);

int fly_prepare_after_accept();

#endif