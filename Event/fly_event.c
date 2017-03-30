/********************************
operation about event.

Author: Andrew lin
********************************/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include "fly_event.h"

fly_core *fly_core_init()
{
    fly_core *core;
    if ((core = malloc(sizeof(fly_core))) == NULL) {
        printf("malloc error.\n");
    	return NULL;
    }
    memset(core,0,sizeof(fly_core));

    core->fly_reg_queue = fly_init_queue();
    core->fly_active_queue = fly_init_queue();
    core->fly_io_queue = fly_init_queue();
    if (core->fly_reg_queue == NULL || core->fly_active_queue == NULL || core->fly_io_queue == NULL) {
	    return NULL;
    }
    
    if ((core->ep_info = malloc(sizeof(struct epoll_info))) == NULL) {
    	printf("malloc error.\n");
    	return NULL;
    }
    memset(core->ep_info,0,sizeof(struct epoll_info));

    if ((core->ep_info->epoll_fd = epoll_create(32000)) == -1) {
	    printf("epoll create error.\n");
	    return NULL;
    } else {
        printf("epoll create successfully,epoll_fd: %d\n",core->ep_info->epoll_fd);
    }

    if ((core->ep_info->events = malloc(100*sizeof(struct epoll_event))) == NULL) {
	    printf("epoll create error.\n");
	    return NULL;
    }
    core->ep_info->nevents = 100;
    return core;
}

int fly_event_set(int fd,void (*callback)(int,void *),fly_event *ev,int flags,void *arg,fly_core *core)
{
    if (callback == NULL || ev == NULL  || core == NULL) {
        if (callback == NULL) {
            printf("callback null.\n");
        } else if (ev == NULL) {
            printf("ev null.\n");
        } else if (core == NULL) {
            printf("core null.\n");
        }

	    return -1;
    }
    ev->fd = fd;
    ev->callback = callback;
    ev->flags = flags;
    ev->arg = arg;
    ev->core = core;
    ev->status = FLY_LIST_REG;
    return 0;
}

int fly_event_add(fly_event *ev) 
{
    if (ev == NULL) {
    	return -1;
    }

    int ret = 0;
    if (ev->status & FLY_LIST_REG) {
    	ret = fly_insert_queue(ev->core->fly_reg_queue,ev) == -1?-2:0;
    	switch (ret) {   	
    		case  0:
    		    ev->status = FLY_LIST_ACTIVE;
    		 	break;
    		case -2:
    		default:
    	    	break;
    	}
    	
    } else if (ev->status & FLY_LIST_ACTIVE) {
    	ret = fly_insert_queue(ev->core->fly_active_queue,ev) == -1?-2:0;
    	switch (ret) {    	
    		case  0:
    		    ev->status = FLY_LIST_PROCESS;
    		 	break;
    		case -2:
    		default:
    	    	break;
    	}
    } else {
    	ret = -3;
    }
    
    return ret;
}

//todo: 1.make fly_queue support delete ele anywhere!
//      2.should del ele at epoll too
int fly_event_del(fly_event *ev) 
{
    if (ev == NULL) {
	return -1;
    }

    int ret = 0;
	switch (ev->status) {
    	case FLY_LIST_ACTIVE:
    	    ret = fly_delete_queue(ev->core->fly_reg_queue,ev) != 1?-2:1;
    	    break;
    	case FLY_LIST_PROCESS:
    	    ret = fly_delete_queue(ev->core->fly_active_queue,ev) != 1?-2:1;
    	    break;
    	case FLY_LIST_REG:
        default:
            ret = -3;
            break;
	}

	return ret;
}

void fly_core_cycle(fly_core *core) 
{
    while(1) {
        if (core == NULL) {
            printf("core is NULL.\n");
            return;
        }

        if (fly_event_add_to_epoll(core) == 0) {
            //no event add to epoll
            //printf("no events add to epoll.\n");
        }
        
        if (fly_queue_empty(core->fly_io_queue) != 1) {
            if (fly_event_dispatch(core) != 0) {
                return;
            }
        } else {
            //printf("no events add to epoll.\n");
        }
        
    
        if (fly_queue_empty(core->fly_active_queue) != 1) {
            //todo: process the active events in active queue.
            printf("active queue is not empty, process active event.\n");
            fly_process_active(core);
        }
    }
    
}

//just add all the events existed in the reg queue without any other logic.(may has problems)
//todo:avoid one event add twice or more times!
int fly_event_add_to_epoll(fly_core *core)
{
    fly_event *event;
    struct epoll_event epev;
    int op;
    int num = 0;

	if (fly_queue_empty(core->fly_reg_queue) == 1) {
    	//reg queue is empty,no need to add event to epoll,just return
        //printf("the reg queue is empty.\n");
    	return 0;
	}

	//add all events to epoll from reg queue
	for (int i = 0; i < fly_queue_length(core->fly_reg_queue); i++) {
        printf("the queue length is: %d.\n", fly_queue_length(core->fly_reg_queue));
		event = fly_pop_queue(core->fly_reg_queue);

    	if (event == NULL) {
    		printf("event popped from queue is NULL.\n");
    		//if get event is NULL,we just ignore it and go continue.
    		continue;
    	}
        if (fly_insert_queue(core->fly_io_queue, event) != 0) {
            printf("add event to I/O queue error.\n");
            return -1;
        }
        if (event->flags & (FLY_EVENT_READ | FLY_EVENT_WRITE)) {
        	if (event->flags & FLY_EVENT_READ) {
        		op = EPOLL_CTL_ADD;
        	    epev.data.fd = event->fd;
        	    epev.events = EPOLLIN; //just care about this event's read event
        	} else if (event->flags & FLY_EVENT_WRITE) {
        		op = EPOLL_CTL_ADD;
        		epev.data.fd = event->fd;
        		epev.events = EPOLLOUT; //just care about this event's write event
        	}
            //we use the default LT for epoll. 
            printf("epoll_fd: %d, op: %d, fd: %d epev.data.fd: %d, epev.events: %d\n",core->ep_info->epoll_fd, op, event->fd, epev.data.fd, epev.events);
        	if (epoll_ctl(core->ep_info->epoll_fd, op, event->fd, &epev) == -1) {
        		perror("epoll_ctl error.\n");
        		continue;
        	} else {
                printf("add a event to epoll.\n");
            }
        	num++;
        }
	}
    
	return num;
}

int fly_event_remove_from_epoll(fly_event *event) 
{
    struct epoll_event epev;
    epev.data.fd = event->fd;
    if (event->flags & FLY_EVENT_READ) {
        epev.events = EPOLLIN;
    } else if (event->flags & FLY_EVENT_WRITE) {
        epev.events = EPOLLOUT;
    } else {
        printf("what's this event?\n");
    }
    if (epoll_ctl(event->core->ep_info->epoll_fd, EPOLL_CTL_DEL, event->fd, &epev) == -1) {
        printf("epoll del error.\n");
        return -1;
    }

    return 1;
}

//todo:incomplete
int fly_event_dispatch(fly_core *core)
{
    fly_event *ev;
    qHead *head = core->fly_io_queue;
    if (head == NULL) {
        printf("queue head error.\n");
        return -1;
    }

    int nfds = epoll_wait(core->ep_info->epoll_fd, core->ep_info->events, core->ep_info->nevents, 200);

    if (nfds < 0) {
        if (errno != EINTR) {
            perror("epoll wait error.\n");
            return -1;
        }

        return 0;
    }
    	

    /*
      in linux epoll:
      typedef union epoll_data {  
        void *ptr;  
        int fd;  
        __uint32_t u32;  
        __uint64_t u64;  
      } epoll_data_t;  
    
      struct epoll_event {  
        __uint32_t events;  //Epoll events   
        epoll_data_t data;  //User data variable 
      };
    */
    //printf("nfds: %d\n",nfds);
    for (int i = 0; i < nfds; i++) {
        int what = core->ep_info->events[i].events;

        if ((ev = fly_use_fd_find_event(core->ep_info->events[i].data.fd, head)) == NULL) {
                printf("fly_use_fd_find_event return NULL.\n");
                return -1;
        }

        if (what & EPOLLIN) {
            /*
              I/O became readable,
              add active to active queue.
            */            
            if (fly_event_add(ev) != 1) {
                //if error,ignore it.
                //printf("event add to queue error.\n");
                continue;
            }
        } else if (what & EPOLLOUT) {
            /*
              i/o become writeable,
              add to active queue.
            */
            if (fly_event_add(ev) != 1) {
                //printf("event add to queue error.\n");
                continue;
            }
        } else {
            continue;
        }
    }

    return 0;
}

fly_event *fly_use_fd_find_event(int fd, qHead head)
{
    fly_event *ev;
    if (head == NULL || fd < 0) {
        printf("queue head is NULL or fd < 0.\n");
        return NULL;
    }

    qPtr qp = head->first;
    if (qp == NULL) {
        printf("qp is NULL.\n");
        return NULL;
    }

    for (; qp != NULL; qp = qp->next) {
        ev = qp->ele;
        if (fd == ev->fd) {
            return qp->ele;
        } else {
            continue;
        }
    }

    return NULL;
}

int fly_process_active(fly_core *core)
{
    fly_event *ev;
    if (core == NULL) {
        printf("core is NULL.\n");
        return -1;
    }
    
    qPtr qp = NULL;
    for (qp = core->fly_active_queue->first; qp != NULL; qp = qp->next) {
        ev = qp->ele;
        if ( ev == NULL) {
            printf("ev is NULL.\n");
            continue;
        } 
        printf("call evevnt's callback.\n");
        (*ev->callback)(ev->fd,ev->arg);
        /*
            todo: should support the persist event.
            1.if persist event,just remove from active queue.
            2.if not persist,remove from active queue and I/O queue.
        */
        if ((fly_delete_queue(core->fly_active_queue, ev) != 1) || fly_delete_queue(core->fly_io_queue, ev) != 1) {
            printf("remove ele from active queue/io queue error.\n");
            return -1;
        } else {
            printf("remove unpersist event successfully.\n");
        }

    }


    return 0;
}

void fifo_read() 
{
    printf("fifo_read: is here.\n");
}

/*

test code.

void main() 
{
    fly_event event;
    static int fds[2];

    //epoll have not support file's fd and directory's fd
    if (pipe(fds)) {
        perror("create pipe error.\n");
        return;
    }

    fly_core *core = fly_core_init();
    if (core == NULL) {
        printf("fly_core_init error.\n");
        return;
    }
    
    //event = malloc(sizeof(struct fly_eventt));
    //if (event == NULL) {
    //    printf(malloc error.\n);
    //    return;
    //}
    

    if (fly_event_set(fds[0], fifo_read, &event, FLY_EVENT_READ, &event, core) == -1) {
        printf("fly_event_set error.\n");
        return;
    }
    
    fly_event_add(&event);
    
    //test code: write
    //we just only write fds[1] but can't write fds[0]
    if (write(fds[1], "c", 1) != 1) {
        perror("write error.\n");
    }

    fly_core_cycle(core);
    return;
}
*/



