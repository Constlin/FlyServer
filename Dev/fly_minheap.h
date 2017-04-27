/********************************
A min-heap implemented based on an array.

insert strategy: new node is inserted at the end of the array.

Author: Andrew lin
********************************/
#ifndef _FLY_MINHEAP_H
#define _FLY_MINHEAP_H

#include <sys/time.h>
#include "fly_event.h"

//typedef is interpreted at compile time, so we need to typedef again in this file to avoid compile error.
typedef struct fly_event* fly_event_p;

struct fly_minheap {
	//an array and the element is the fly_event_p, support dynamic growth.
	fly_event_p *fly_event;
	//min-heap's size.
	int fly_minheap_size; 
	//min-heap's capacity.
	int fly_minheap_cap;
};

typedef struct fly_minheap fly_minheap_t;

typedef struct fly_minheap* fly_minheap_p;

fly_minheap_p fly_minheap_init(fly_minheap_p ptr);

int fly_minheap_size(fly_minheap_p ptr);

//make sure that we have enough memory to store the timeout events.
int fly_minheap_reserve(fly_minheap_p ptr, int size);

/*
  index is the index of the array that which place we should insert the new event.
  the index is the array's end.
  index is from 0 to fly_minheap_cap - 1.
*/
int fly_minheap_insert(fly_minheap_p ptr, fly_event_p event, int index);

int fly_minheap_push(fly_minheap_p ptr, fly_event_p event);

fly_event_p fly_minheap_top(fly_minheap_p ptr);

/*
  remove the top element ptr->fly_event pointed and move the left element forward.
*/
int fly_minheap_pop(fly_minheap_p ptr);

//get the top evetn and free it.
int fly_minheap_free_top(fly_minheap_p ptr);

//move the last one to the top, and set the last one's ptr is NULL.
int fly_minheap_move_last_to_top(fly_minheap_p ptr);

/*  adjust the minheap, after move the last one to the top.
    adjust way:if top less than his left and right child, we do nothing.
    if less both than left and right, we choose left. Other situation,
    choose the less one.
*/
int fly_minheap_top_adjust(fly_minheap_p ptr, int index);

int fly_minheap_search(fly_minheap_p ptr, fly_event_p event);

int fly_minheap_set_top_null(fly_minheap_p ptr);

//make fly_minheap's time decreace top's time to make sure that epoll_wait get true timeout.
int fly_minheap_time_adjust(fly_minheap_p ptr);

#endif
