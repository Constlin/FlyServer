#ifndef _FLY_CONNECTION_H
#define _FLY_CONNECTION_H

struct fly_listening {
    //the listening fd.
    int                 fd;

    //listen fd相关的sockaddr.
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

};

typedef struct fly_listening fly_listening_t;

struct fly_connection {
	//the sockef fd that this fly_connection assicuated.
    int fd;

    //the read event about this fly_connection.
    struct fly_event *read;

    //the write event about this fly_connection.
    struct fly_event *write;

    //the buffer used for read this connection's internet data.
    struct fly_buf *read_buf;

    //the buffer used for write data to this connection's internet.
    struct fly_buf *write_buf;
};

typedef struct fly_connection fly_connection_t;

#endif