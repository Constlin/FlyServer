/********************************
operation about event.

Author: Andrew lin
********************************/
#ifndef _FLY_EVENT_H
#define _FLY_EVENT_H

#include "fly_queue.h"

#define FLY_LIST_REG 0x01
#define FLY_LIST_ACTIVE 0x02
#define FLY_LIST_PROCESS 0x04

#define FLY_EVENT_READ 0x01
#define FLY_EVENT_WRITE 0x02

#define FLY_CTL_ADD 0x01
#define FLY_CTL_DEL 0x02
#define FLY_CTL_MOD 0X04

typedef struct fly_event fly_event;

typedef struct fly_core fly_core;

struct fly_event {
    int fd;
    /*
        the thing that the event take care. 
	FLY_EVENT_READ mean I/O read event,
	FLY_EVENT_WRITE mean I/O write event.
    */
    int flags;
    /*
      the event next in which queue.
      FLY_LIST_REG mean need to add in registered queue,      
      FLY_LIST_ACTIVE mean it's in registered queue and need to add in active queue,
      FLY_LIST_PROCESS mean it's in active queue and wait for processing.
    */
    int status;
    /*
      FLY_CTL_ADD mean this event need to  added to epoll,      
      FLY_CTL_DEL mean this event need to modify from epoll,
      FLY_CTL_MOD mean this event need to del from epoll,
    */
    int do_what;
    /*
      para of callback
    */
    void *arg;
    /*
      callback for the event
    */
    void (*callback)(int,void *);

    fly_core *core;
};

struct epoll_info {
    int epoll_fd;
    struct epoll_event *events;
    int nevents;
};

struct fly_core {
	  /*
      registered queue, save the event needed to add into epoll
    */
    struct fly_queue_head *fly_reg_queue;
    /*
      active queue
    */
    struct fly_queue_head *fly_active_queue;
    /*
      I/O even queue which save the I/O event has added into epoll
    */
    struct fly_queue_head *fly_io_queue;
    /*
      store the information about epoll
    */
    struct epoll_info *ep_info;
};


fly_core *fly_core_init();

int fly_event_set(int fd,void (*callback)(int,void *),fly_event *ev,int flags,void *arg,fly_core *core);

int fly_event_add(fly_event *ev);

//remove event from reg queue
int fly_event_del(fly_event *ev);

/*
    function: add the events from reg queue to epoll and save the regged events at I/O queue,
              and then use epoll_wait this events added in epoll happen,if happen add this 
              events to fly_active_queue and process these active events.
*/
void fly_core_cycle(fly_core *core);

//add event to epoll, return number of events added to the epoll
int fly_event_add_to_epoll(fly_core *core);

//remove event from epoll
int fly_event_remove_from_epoll(fly_event *event);

//use epoll,if I/O event is active,add it from reg queue to active queue.
int fly_event_dispatch(fly_core *core);

//process the active events in active queue.
int fly_process_active(fly_core *core);

//use fd to find event in reg queue.
fly_event *fly_use_fd_find_event(int fd,qHead head);

//test method
void fifo_read();

#endif
