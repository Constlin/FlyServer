#include <stdio.h>
#include <malloc.h>
#include "queue.h"
/********************************
A fifo queue implemented based on a list.

Author: Andrew lin
********************************/



//initializate a queue
void init_queue(qHead queue) 
{
    queue = malloc(sizeof(struct queue_head));
    if (!queue) {
    	printf("malloc error.\n")
    }
    queue->first = NULL;
    queue->last = NULL;
    return;
}

//insert a ele to a queue
void insert_queue(qHead queue,void *ele)
{
    assert(ele != NULL && queue != NULL);
    
    qPtr qptr = malloc(sizeof(struct queue_node));
    if (!qprt) {
    	printf("malloc error.\n")
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
    return;
}

//get the first queue and remove it from queue
void *pop_queue(qHead queue)
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

//destroy the queue
void delete_queue(qHead queue) 
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
