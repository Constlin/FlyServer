/*****************************
Below is test code for I/O event.
 
author: Andrew Lin
*****************************/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fly_event.h"

static int fds[2];

void fifo_read() 
{
    printf("fifo_read: is here.\n");
    char *buf = malloc(10);

    if (buf == NULL) {
        printf("malloc error.\n");
        return;
    }

    memset(buf,'\0',strlen(buf));

    int n = 0, nread = 0;

    if (fds[0] == 0) {
        printf(" fds[0] is null.\n");
        return;
    }

    //
    //  because the epoll mode is ET, so we should make sure that read all the data that we can read
    //  from the read buffer.
    //  for system method read,the return value:
    //  1.ret < 0, and set the errno: if errno == EAGAIN, means no data to read.if errno is others,always means error happen.
    //  2.ret = 0: mean the file at the tail and no data for reading
    //  3.ret > 0: the bytes that we have readed
    //
    while (1) {
        nread = read(fds[0], buf + nread, 10 - nread);
        
        if (nread < 0) {
            if (errno == EAGAIN) {
                //nread < 0 and errno = EAGAIN means that the read buffer has no data to read,
                //we just mark it as read successfully and break this while loop.
                break;
            } else {
                return;
            }
        } else if (nread == 0) {
            //it means that the file is at tail or there is no data for reading.
            break;
        } else {
            //there is data has been readed.
            n = n + nread;
            if (n == 10) {
                break;
            } else {
                continue;
            }
        }
    }

    printf("buf: %s\n",buf);
}


void test_io_main() 
{
    fly_event event;
    int flag1 = 0, flag2 = 0;

    //epoll have not support file's fd and directory's fd
    if (pipe(fds)) {
        perror("create pipe error.\n");
        return;
    }
    
    flag1 = fcntl(fds[0], F_GETFL, 0);
    flag2 = fcntl(fds[1], F_GETFL, 0);

    flag1 |= O_NONBLOCK;
    flag2 |= O_NONBLOCK;

    if(fcntl(fds[0], F_SETFL, flag1) < 0) {
        perror("fcntl error.\n");
        return;
    }

    if(fcntl(fds[1], F_SETFL, flag2) < 0) {
        perror("fcntl error.\n");
        return;
    }

    fly_core *core = fly_core_init();
    if (core == NULL) {
        printf("fly_core_init error.\n");
        return;
    }
    
    //should we malloc for the event?
    if (fly_event_set(fds[0], fifo_read, &event, FLY_EVENT_READ, &fds[0], core, NULL) == -1) {
        printf("fly_event_set error.\n");
        return;
    }
    
    fly_event_add(&event);
    
    //test code: write
    //we just only write fds[1] but can't write fds[0]
    if (write(fds[1], "abcdefghij", 10) != 10) {
        perror("write error.\n");
    }

    fly_core_cycle(core);
    return;
}
