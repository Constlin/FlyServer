/********************************
A min-heap implemented based on an array.

insert strategy: new node is inserted at the end of the array.

Author: Andrew lin
********************************/
#include <malloc.h>
#include <stdio.h>
#include "fly_minheap.h"
#include "fly_util.h"

fly_minheap_p fly_minheap_init(fly_minheap_p ptr)
{
    ptr = malloc(sizeof(struct fly_minheap));

    if (ptr == NULL) {
    	printf("[ERROR] malloc error.\n");
    	return NULL;
    }

    ptr->fly_event = NULL;
    ptr->fly_minheap_size = 0;
    ptr->fly_minheap_cap =0;

    return ptr;
}

int fly_minheap_size(fly_minheap_p ptr)
{
	return ptr->fly_minheap_size;
}

int fly_minheap_reserve(fly_minheap_p ptr, int need_size)
{
	if (ptr == NULL) {
		printf("[ERROR] fly_minheap is NULL.\n");
		return -1;
	}

	if (ptr->fly_minheap_cap < need_size) {
		//the capasity is less than needed size, so we need to more memory.
		fly_event_p *temp_event;
		/*
            if first reserve, we set the capasity is 8. if not, double.
		*/
		int temp_cap = ptr->fly_minheap_cap ? ptr->fly_minheap_cap * 2 : 8;

		if (temp_cap < need_size) 
			temp_cap = need_size;
		
		if(!(temp_event = (fly_event_p *)realloc(ptr->fly_event, temp_cap * sizeof(*temp_event)))) {
			printf("[ERROR] realloc error.\n");
			//todo: should free the fly_minheap
			return -1;
		}

		ptr->fly_event = temp_event;
		ptr->fly_minheap_cap = temp_cap;
	}

	return 1;
}

int fly_minheap_insert(fly_minheap_p ptr, fly_event_p event, int index)
{
	if (ptr == NULL || event == NULL || index < 0) {
		printf("[ERROR] insert error.\n");
		return -1;
	}

    ptr->fly_event[index] = event;
    //the event's parent event's index.
    int parent = (index - 1) / 2;
    int temp = 0;

    //find the true place to insert.
    //todo: test again, i use 'struct timeval **'' rather 'struct timeval *''
    while(index && (fly_comparetime(ptr->fly_event[parent]->time, event->time) == 1)) {
        //the event's parent's event's time is larger that the event's. so switch it.
        fly_switch(&ptr->fly_event[parent], &event);
        temp = parent;
        index = parent;
        parent = (temp - 1) / 2;
    }
  
    ++ptr->fly_minheap_size;
    printf("[Debug]. add a timeout event: %p to fly_minheap(size: %d) success.\n", event, ptr->fly_minheap_size);
    return 1;   
}

int fly_minheap_push(fly_minheap_p ptr, fly_event_p event) 
{
	if (ptr == NULL || event == NULL) {
		printf("[ERROR] fly_minheap_push error.\n");
		return -1;
	}
 
    if (fly_minheap_search(ptr, event) == 1) {
    	//the event's time is conflict, just return -1.
        printf("[ERROR] the event's time is conflict.\n");
        return -1;
    }

	if (fly_minheap_reserve(ptr, fly_minheap_size(ptr) + 1) < 0) {
		return -1;
	}

	if (fly_minheap_insert(ptr, event, fly_minheap_size(ptr)) < 0) {
		printf("[ERROR] fly_minheap_insert error.\n");
		return -1;
	}

    return 1;
}

fly_event_p fly_minheap_top(fly_minheap_p ptr)
{
	if (ptr->fly_minheap_size > 0) {
		return (ptr->fly_event)[0];
	}
    
	return NULL;
}

//todo: incomplete
int fly_minheap_pop(fly_minheap_p ptr)
{
	printf("[DEBUG] call fly_minheap_pop.\n");

	if (fly_minheap_free_top(ptr) < 0) {
		return -1;
	}

	if (fly_minheap_move_last_to_top(ptr) < 0) {
		return -1;
	}

    if (fly_minheap_top_adjust(ptr, 0) < 0) {
    	return -1;
    }
    
    return 1;
}

int fly_minheap_free_top(fly_minheap_p ptr)
{
	if (fly_minheap_size(ptr) <= 0) {
        printf("[ERROR] the minheap's size is less than 1.\n");
        return -1;
	}
	/*
    printf("fly_minheap's pointer: %p, free: %p.\n", ptr, fly_minheap_top(ptr));
    //no need to free, because the event is not save in heap, it just a local variable.
    free(fly_minheap_top(ptr));
    */
    fly_minheap_set_top_null(ptr);

    return 1;
}

int fly_minheap_move_last_to_top(fly_minheap_p ptr)
{
	if (ptr->fly_event[fly_minheap_size(ptr) - 1] == NULL && fly_minheap_size(ptr) > 1 ) {
		//use fly_minheap_size(ptr) > 1 to avoid this situation:
		//only one event in fly_minheap, we should support to pop this event.
		printf("[ERROR] the minheap's last one is NULL.\n");
		return -1;
	}

    ptr->fly_event[0] = ptr->fly_event[fly_minheap_size(ptr) - 1];

    ptr->fly_event[fly_minheap_size(ptr) - 1] = NULL;
  
    --ptr->fly_minheap_size;

    return 1;
}

int fly_minheap_top_adjust(fly_minheap_p ptr, int index)
{
	printf("come to fly_minheap_top_adjust. index'value: %d.\n", index);

	if (ptr == NULL) {
		return -1;
	}

	if (fly_minheap_size(ptr) < 1) {
		printf("in func fly_minheap_top_adjust: fly_minheap's size < 1. No need to adjust.\n");
		return 1;
	} 

	int parent_index = index;
	int left_index = 2 * parent_index + 1;
	int right_index = 2 * parent_index + 2;
	int left_ret = 0, right_ret = 0;

	fly_event_p parent_event = ptr->fly_event[parent_index];
	fly_event_p left_event = ptr->fly_event[left_index];
	fly_event_p right_event = ptr->fly_event[right_index];

    if (parent_event == NULL) {
    	printf("fly_minheap_top_adjust parent_event is NULL.\n");
    	return -1;
    }

    if (left_event == NULL && right_event == NULL) {
    	//no need to adjust top in this situation.
    	printf("both left_event and right_event are NULL.\n");
    	return 1;

    } else if (left_event != NULL && right_event == NULL) {
    	//todo: test again, i use 'struct timeval **'' rather 'struct timeval *''
        left_ret = fly_comparetime(parent_event->time, left_event->time);

        if (left_ret == 3) {
        	return 1;
        }

        if (left_ret == 1) {
        	fly_switch(&parent_event, &left_event);
        	if (fly_minheap_top_adjust(ptr, left_index) != 1) {
        		return -1;
        	}
        	return 1;
        }

    } else if (left_event == NULL && right_event != NULL) {
    	//todo: test again, i use 'struct timeval **'' rather 'struct timeval *''
    	right_ret = fly_comparetime(parent_event->time, right_event->time);

        if (right_ret == 3) {
        	return 1;
        }

        if (right_ret == 1) {
        	fly_switch(&parent_event, &right_event);

        	if (fly_minheap_top_adjust(ptr, right_index) != 1) {
        		return -1;
        	}

        	return 1;
        }

    } else if (left_event != NULL && right_event != NULL) {
    	//todo: test again, i use 'struct timeval **'' rather 'struct timeval *''
        left_ret = fly_comparetime(parent_event->time, left_event->time);
        //todo: test again, i use 'struct timeval **'' rather 'struct timeval *''
        right_ret = fly_comparetime(parent_event->time, right_event->time);

	    if (left_ret == 3 && right_ret == 3) {
	    	//the top event's time is less than his left and right child.
	    	//we just return without doing anything.
	    	return 1;
	    }
    
	    if (left_ret == 1 && right_ret == 3) {
	    	//the top event's time > left child but < right child.
	    	//we switch the parent one and his left one.
	    	fly_switch(&parent_event, &left_event);

	    	if (fly_minheap_top_adjust(ptr, left_index) != 1) {
	    		return -1;
	    	}

	    	return 1;
	    }
    
	    if (left_ret == 3 && right_ret == 1) {
	    	//switch the parent one and his right one.
	    	fly_switch(&parent_event, &right_event);

	    	if (fly_minheap_top_adjust(ptr, right_index) != 1) {
	    		return -1;
	    	}

	    	return 1;
	    }
    
	    if (left_ret == 1 && right_ret == 1) {
	    	//in this situation, we choose the smaller one between left and right.
	    	//todo: test again, i use 'struct timeval **'' rather 'struct timeval *''
	    	int ret = fly_comparetime(left_event->time, right_event->time);

	    	if (ret == 1) {
	    		//choose right one wo switch with parent one.
	    		fly_switch(&parent_event, &right_event);

	    		if (fly_minheap_top_adjust(ptr, right_index) != 1) {
	    		    return -1;
	    	    }

	    	    return 1;
	    	} 
    
	    	if (ret == 3) {
	    		//choose left one wo switch with parent one.
	    		fly_switch(&parent_event, &left_event);

	    	    if (fly_minheap_top_adjust(ptr, left_index) != 1) {
	    		    return -1;
	    	    }

	    	    return 1;
	    	}
            
            printf("fly_comparetime error or the event's time in minheap has repeated.\n");
	    	return -1;
	    }
    }

	return -1;
}

int fly_minheap_search(fly_minheap_p ptr, fly_event_p event)
{
	if (ptr == NULL || event == NULL) {
		return -1;
	}

	for (int i = 0; i < ptr->fly_minheap_size; i++) {
		if (fly_comparetime((ptr->fly_event[i])->time, event->time) == 2) {
			//time is conflict.
            return 1;
		}
	}

	return 0;
}

int fly_minheap_set_top_null(fly_minheap_p ptr)
{
	if (fly_minheap_size(ptr) < 1) {
		printf("fly_minheap's size < 1.\n");
		return -1;
	}

	(ptr->fly_event)[0] = NULL;
	return 1;
}

int fly_minheap_time_adjust(fly_minheap_p ptr)
{
	if (ptr->fly_minheap_size < 1) {
		printf("[ERROR]. fly_minheap's size < 1.\n");
		return -1;
	}

	fly_event_p top_event = fly_minheap_top(ptr);

	for (int i = 0; i < ptr->fly_minheap_size - 1; ++i) {
        fly_time_sub((ptr->fly_event)[i+1]->time, (ptr->fly_event)[i+1]->time, &(top_event->user_settime));
	}

    printf("test log. fly_minheap's size: %d.\n", ptr->fly_minheap_size);
    
	for(int i = 0; i < ptr->fly_minheap_size; ++i) {
		printf("event's ptr: %p, event's time ptr: %p, test log. tv_sec: %ld, tv_usec: %ld.\n", (ptr->fly_event)[i], (ptr->fly_event)[i]->time, (ptr->fly_event)[i]->time->tv_sec, (ptr->fly_event)[i]->time->tv_usec);
	}

	return 1;
}

int fly_minheap_free(fly_minheap_p ptr)
{
	if (ptr == NULL) {
		printf("[ERROR] fly_minheap_free: fly_minheap is NULL.\n");
		return -1;
	}

	free(ptr);

	ptr = NULL;
	return 1;
}


/******************************************
    below is test code for fly_minheap.
*******************************************/
/*
void time_out()
{
    printf("the time is out.");
}

void main()
{
	fly_core *core = fly_core_init();
    if (core == NULL) {
        printf("fly_core_init error.\n");
        return;
    }

	fly_minheap_p minheap_ptr = NULL;
	minheap_ptr = fly_minheap_init(minheap_ptr);

	if (minheap_ptr == NULL) {
		printf("fly_minheap_init error.\n");
		return;
	}
    
    printf("fly_minheap's size: %d.\n", fly_minheap_size(minheap_ptr));

    fly_event_p event_ptr1 = malloc(sizeof(struct fly_event));
    if (event_ptr1 == NULL) {
    	return;
    }

    fly_event_p event_ptr2 = malloc(sizeof(struct fly_event));
    if (event_ptr2 == NULL) {
    	return;
    }

    struct timeval tv1;
    tv1.tv_sec = 5;
    tv1.tv_usec = 0;

    struct timeval tv2;
    tv2.tv_sec = 10;
    tv2.tv_usec = 0;
    if (fly_event_set(-1, time_out, event_ptr1, FLY_EVENT_READ, NULL, core, &tv1) == -1) {
        printf("fly_event_set error.\n");
        return;
    }

    if (fly_event_set(-1, time_out, event_ptr2, FLY_EVENT_READ, NULL, core, &tv2) == -1) {
        printf("fly_event_set error.\n");
        return;
    }

	fly_minheap_push(minheap_ptr, event_ptr1);
    //fly_minheap_pop(minheap_ptr);
	//fly_minheap_push(minheap_ptr, event_ptr2);
	printf("fly_minheap's size: %d.\n", fly_minheap_size(minheap_ptr));

	
	//fly_minheap_pop(minheap_ptr);
	printf("fly_minheap's size: %d.\n", fly_minheap_size(minheap_ptr));
    
    printf("enter fly_core_cycle.\n");
    fly_core_cycle(core);
	return;
} //end of the fly_minheap's test code.
*/