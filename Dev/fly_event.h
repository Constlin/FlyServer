/********************************
operation about event.

Author: Andrew lin
********************************/
#ifndef _FLY_EVENT_H
#define _FLY_EVENT_H

#include <sys/time.h>
#include "fly_queue.h"
#include "fly_minheap.h"

#define FLY_LIST_REG 0x01
#define FLY_LIST_ACTIVE 0x02
#define FLY_LIST_PROCESS 0x04
#define FLY_MINHEAP_REG 0X08

#define FLY_EVENT_READ 0x01
#define FLY_EVENT_WRITE 0x02
#define FLY_EVENT_SIG 0x04
#define FLY_EVENT_INTERNAL 0x08
#define FlY_EVENT_UNPERSIST 0x16

#define FLY_CTL_ADD 0x01
#define FLY_CTL_DEL 0x02
#define FLY_CTL_MOD 0X04

//todo: remove this typedef 'typedef struct fly_event fly_event;'
typedef struct fly_event fly_event;

typedef struct fly_event fly_event_t;

typedef struct fly_event* fly_event_p;

typedef struct fly_core fly_core;

typedef struct fly_core* fly_core_p;

struct fly_event {
    int fd;
    /*
      if set this value, it means this event is a timeout event, and the value is the time.
      else it should be 0;
    */
    struct timeval *time;
    /*
      use to adjust the fly_heap's timeoutevent's time when process a timeout event over.
    */
    struct timeval user_settime;
    
    struct timeval current_time_cache;
    /*
      the thing that the event take care. 
	    FLY_EVENT_READ mean I/O read event,
	    FLY_EVENT_WRITE mean I/O write event,
      FLY_EVENT_SIG mean signal event,
      FLY_EVENT_INTERNAL mean this is internal event.
    */
    int flags;
    /*
      the event next in which queue.
      FLY_LIST_REG mean need to add in registered queue,      
      FLY_LIST_ACTIVE mean it's in registered queue and need to add in active queue,
      FLY_LIST_PROCESS mean it's in active queue and wait for processing.
      FLY_MINHEAP_REG mean it's a timeout event and need to add to minheap.
    */
    int status;
    /*
      para of callback
    */
    void *arg;
    /*
      callback for the event
    */
    void (*callback)(int fd, void *arg);

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
    /*
      store the timeout event that have not added into the epoll.
    */
    struct fly_minheap *fly_timeout_minheap;
    /*
      internal event used for adding signal to fly_core.
    */
    struct fly_event fly_evsig;
    /*
      socketpair used for adding signal to fly_core.
      fly_socketpair[0] care read side,
      fly_socketpair[1] care write side.
    */  
    int fly_socketpair[2];
    /*
      a hash table, we can use signal number to get the events from the hash table.
    */
    struct fly_hash *fly_hash;
    //the return from the method fly_sig_init. 0 for success, -1 for fail.
    int sig_ret;
    
};


fly_core *fly_core_init();

//one struct timeval can't use more than one time.
int fly_event_set(int fd, void (*callback)(int fd, void *arg), fly_event *ev, int flags, void *arg, fly_core *core, struct timeval *tv);

//can't add one event twice! it will cause the event's status uncertain!
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
fly_event *fly_use_fd_find_event(int fd, qHead head);

long fly_event_get_timeout(fly_core *core);

int fly_process_timeout(fly_core *core);

void fly_core_clear(fly_core *core);

#endif
