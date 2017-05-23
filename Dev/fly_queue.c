/********************************
A fifo queue implemented based on a list.

Author: Andrew lin
********************************/
#include <stdio.h>
#include <malloc.h>
#include "fly_queue.h"


//initializate a queue
qHead fly_init_queue() 
{
    qHead queue = malloc(sizeof(struct fly_queue_head));

    if (!queue) {
    	printf("malloc error.\n");
        return NULL;
    }
    
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}

//insert a ele to a queue
int fly_insert_queue(qHead queue, void *ele)
{
    if (queue == NULL || ele == NULL) {
        return -1;
    }
    
    qPtr qptr = malloc(sizeof(struct fly_queue_node));

    if (!qptr) {
    	printf("malloc error.\n");
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
	if (queue == NULL) {
        return NULL;
    }

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

int fly_delete_queue(qHead queue, void *ele)
{
    if (queue == NULL || ele == NULL) {
        printf("queue or ele NULL.\n");
        return -1;
    }

    qPtr pt = queue->first;
    qPtr pt_before = queue->first;  
    qPtr pt_next = pt->next;
    int ret = 0;

    for (; pt != NULL; pt_before = pt, pt = pt->next) {
        if (pt->ele == ele && pt == pt_before) {           
            //head
            free(pt);

            if (pt_next == NULL) {
                queue->first = queue->last = NULL;
                return 1;
            }

            queue->first = pt_next;
            ret = 1;
            break;           
        } else if (pt->ele == ele && pt != pt_before) {
            if (!pt->next) {
                //pt is the last one
                free(pt);
                queue->last = pt_before;
                queue->last->next = NULL;
                ret = 1;
                break;
            } else {
                //pt is not the last one
                pt_next = pt->next;
                free(pt);
                pt_before->next = pt_next;
                ret = 1;
                break;
            }
        }             
    }

    return ret;
}

int fly_queue_empty(qHead queue)
{
    return queue->first == NULL? 1: -1;
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
int fly_destroy_queue(qHead queue) 
{
    if (queue == NULL) {
        return -1;
    }

    qPtr current = NULL;
    qPtr item;

	for (qPtr item = queue->first; item != NULL; item = item->next) {
		current = item;
		free(current);
        current = NULL;
	}
    
    free(queue);
    queue = NULL;

	return 1;
}

void *fly_queue_get_top(qHead queue)
{
    if (fly_queue_empty(queue) == 1) {
        return NULL;
    }

    return queue->first->ele;
}

int fly_clear_queue(qHead queue)
{
    if (queue == NULL) {
        return -1;
    }

    qPtr current = NULL;
    qPtr item;

    for (qPtr item = queue->first; item != NULL; item = item->next) {
        current = item;
        free(current);
        current = NULL;
    }

    return 1;
}

/*
    the main's function is testing fly_queue.
    test case:
    1.insert node
    2.delete node at first, last, middle location
    3.destroy the queue
*/

/*
int main() 
{
    qPtr temp_queuenode;
    int ele1 = 100, ele2 = 200, ele3 = 300;
    qHead test_head= fly_init_queue();
    fly_insert_queue (test_head,&ele1);
    fly_insert_queue (test_head,&ele2);
    fly_insert_queue (test_head,&ele3);
    printf("the queue length is: %d.\n",fly_queue_length(test_head));
    for (temp_queuenode = test_head->first; temp_queuenode != NULL; temp_queuenode = temp_queuenode->next) {
        printf("the ele is : %d \n",*((int*)temp_queuenode->ele));
    }
    
    fly_delete_queue(test_head,&ele1);
    printf("the queue length is: %d.\n",fly_queue_length(test_head));
    //free(test_head);
    //test_head = NULL;


    for (temp_queuenode = test_head == NULL? NULL: test_head->first; temp_queuenode != NULL ; temp_queuenode = temp_queuenode->next) {
        printf("the ele is : %d \n",*((int*)temp_queuenode->ele));
    }
    return 1;
}
*/
