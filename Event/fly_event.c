
/********************************
operation about event.

Author: Andrew lin
********************************/
#include "fly_event.h"

fly_core *fly_core_init()
{
    fly_core *core;
    if ((core = malloc(sizeof(fly_core))) == NULL) {
        printf("malloc error.\n")
    	return NULL;
    }
    memset(core,0,sizeof(fly_core));

    core->fly_reg_queue = fly_init_queue();
    core->fly_active_queue = fly_init_queue();
    if (core->fly_reg_queue == NULL || core->fly_active_queue == NULL) {
	return NULL;
    }
    
    if ((core->ep_info = malloc(sizeof(struct epoll_info))) == NULL) {
    	printf("malloc error.\n")
    	return NULL;
    }
    memset(core->ep_info,0,sizeof(struct epoll_info));

    if ((core->ep_info->epoll_fd = epoll_create(32000)) == -1) {
	printf("epoll create error.\n");
	return NULL;
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
    if (callback == NULL || ev == NULL || arg == NULL || core == NULL) {
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
    	ret = fly_insert_queue(core->fly_reg_queue,ev) == NULL?-2:1;
    	switch (ret) {   	
    		case  0:
    		    ev->status = FLY_LIST_ACTIVE;
    		 	break;
    		case -2:
    		default:
    	    	break;
    	}
    	
    } else if (ev->status & FLY_LIST_ACTIVE) {
    	ret = fly_insert_queue(core->fly_active_queue,ev) == NULL?-2:1;
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
    	    ret = fly_delete_queue(core->fly_reg_queue,ev) == NULL?-2:1;
    	    break;
    	case FLY_LIST_PROCESS:
    	    ret = fly_delete_queue(core->fly_active_queue,ev) == NULL?-2:1;
    	    break;
    	case FLY_LIST_REG:
        default:
            ret = -3;
            break;
	}

	return ret;
}

//todo:incomplete
void fly_core_cycle(fly_core *core) 
{
    while(1) {
        if (core == NULL) {
            printf("core is NULL.\n");
            return;
        }

        if (!fly_event_add_to_epoll(core)) {
            //no event add to epoll
            printf("no events add to epoll.\n");
        }

        if (!fly_event_dispatch(core)) {
            return;
        }
    
        if (!fly_queue_empty(core->fly_active_queue)) {
            //todo: process the active events in active queue.
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

	if (fly_queue_empty(core->fly_reg_queue)) {
    	//reg queue is empty,no need to add event to epoll,just return
    	return 0;
	}

	//add all events to epoll from reg queue
	for (int i = 0 ; i<fly_queue_length(core->fly_reg_queue) ; i++) {
		event = fly_pop_queue(core->fly_reg_queue);
    	if (event = NULL) {
    		printf("event popped from queue is NULL.\n");
    		//if get event is NULL,we just ignore it and go continue.
    		continue;
    	}
        
        if (event->flags & (FLY_EVENT_READ|FLY_EVENT_WRITE)) {
        	if (evevt->flags & FLY_EVENT_READ) {
        		op = EPOLL_CTL_ADD;
        	    epev->data.fd = event->fd;
        	    epev->events = EPOLLIN; //just care about this event's read event
        	} else if (event->flags & FLY_EVENT_WRITE) {
        		op = EPOLL_CTL_ADD;
        		epev->data.fd = event->fd;
        		epev->events = EPOLLOUT; //just care about this event's write event
        	}
            //we use the default LT for epoll. 
        	if (epoll_ctl(core->epoll_fd,op,event->fd,&epev) == -1) {
        		printf("epoll_ctl error.\n");
        		continue;
        	}
        	num++;
        }
	}

	return num;
}

bool fly_event_remove_from_epoll(fly_event *event) 
{
    struct epoll_event epev;
    epev.data.fd = event.fd;
    if (evevt->flags & FLY_EVENT_READ) {
        epev->events = EPOLLIN;
    } else if (event->flags & FLY_EVENT_WRITE) {
        epev->events = EPOLLOUT;
    } else {
        printf("what's this event?\n");
    }
    if (epoll_ctl(event->core->epoll_fd,EPOLL_CTL_DEL,event->fd,&epev) == -1) {
        printf("epoll del error.\n");
        return false;
    }

    return true;
}

//todo:incomplete
int fly_event_dispatch(fly_core *core)
{
    fly_event *ev;
    qHead *head = core->fly_reg_queue;
    if (head == NULL) {
        printf("queue head error.\n");
        return -1;
    }

    int nfds = epoll_wait(core->ep_info->epoll_fd,core->ep_info->events,core->ep_info->nevents,200);

    if (nfds = -1) {
    	printf("epoll wait error.\n");
    	return -1;
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
    for (int i = 0; i < nfds; i++) {
        int what = core->ep_info->events[i].events;

        if (ev = fly_use_fd_find_event(core->ep_info->events[i].data.fd,head) == NULL) {
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
                printf("event add to queue error.\n");
                continue;
            }
        } else if (what & EPOLLOUT) {
            /*
              i/o become writeable,
              add to active queue.
            */
            if (fly_event_add(ev) != 1) {
                printf("event add to queue error.\n");
                continue;
            }
        } else {
            continue;
        }
    }

    return 0;
}

fly_event *fly_use_fd_find_event(int fd,qHead *head)
{
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
        if (fd == qp->ele->fd) {
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
        (*ev->callback)(ev->fd,ev->arg);
    }

    return 0;
}



