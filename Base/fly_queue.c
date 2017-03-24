/********************************
A fifo queue implemented based on a list.

Author: Andrew lin
********************************/
#include <stdio.h>
#include <malloc.h>
#include "queue.h"

//initializate a queue
void* fly_init_queue() 
{
    qHead queue = malloc(sizeof(struct queue_head));
    if (!queue) {
    	printf("malloc error.\n")
        return NULL;
    }
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}

//insert a ele to a queue
int fly_insert_queue(qHead queue,void *ele)
{
	assert(ele != NULL && queue != NULL);
    
    qPtr qptr = malloc(sizeof(struct queue_node));
    if (!qprt) {
    	printf("malloc error.\n")
        return -1;
    }
    qptr->ele = ele;
    qptr->next = NULL;

    if (!queue->first) {
    	//first insert
    	queue->first = qptr;
    	queue->last = qptr;
    } else {
    	queue->last->next = qptr;
    	queue->last = qptr;
    }
    return 0;
}

//get the first queue and remove it from queue
void *fly_pop_queue(qHead queue)
{
	assert(queue != NULL);

	if (queue->first) {
		void *temp = queue->first;
		void *tptr = queue->first->ele;
		queue->first = queue->first->next;
		free(temp);	
		return tptr;
	} else {
		return NULL;
	}
}

void fly_delete_queue(qHead queue,void *ele)
{

}

bool fly_queue_empty(qHead queue)
{
    return queue->first == NULL?true:false;
}

int fly_queue_length(qHead queue)
{
    qPtr temp = queue->first;
    int len = 0;
    while(temp != NULL) {
        len++;
        temp = temp->next;
    }

    return len;
}

//destroy the queue
void fly_destroy_queue(qHead queue) 
{
	assert(queue != NULL);
    qPtr temp = NULL;
    qPtr temp_another;
	for (qPtr temp_another = queue->first ; temp_another != NULL ; temp_another = temp_another->next) {
		temp = temp_another;
		free(temp);
	}
	return;
}
