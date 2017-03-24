/********************************
A fifo queue implemented based on a list.

Author: Andrew lin
********************************/
#ifndef _FLY_QUEUE_H
#define _FLY_QUEUE_H

struct fly_queue_node {
	void              *ele;
	struct fly_queue_node *next;
};

typedef struct fly_queue_node  qNode;
typedef struct fly_queue_node* qPtr;

struct fly_queue_head {
	qPtr first;
	qPtr last;
};

typedef struct fly_queue_head* qHead;


//initializate a queue
void* fly_init_queue();

//insert a ele to a queue
int fly_insert_queue(qHead queue,void *ele);

//get the first queue and remove it from queue
void *fly_pop_queue(qHead queue);

//delete ele from queue anywhere
void fly_delete_queue(qHead queue,void *ele);

//reture weather the queue is empty or not
bool fly_queue_empty(qHead queue);

//get queue length
int fly_queue_length(qHead queue);

//destroy the queue
void fly_destroy_queue(qHead queue);

#endif
