/********************************
operation about dynamic array

Author: Andrew lin
********************************/
#include <stdio.h>
#include <stdlib.h>
#include "fly_array.h"

fly_array_t *fly_array_init()
{
    fly_array_t *dynamic_array = malloc(sizeof(fly_array_t));

    if (dynamic_array == NULL) {
    	printf("[ERROR] fly_array_init: malloc error.\n");
    	return NULL;
    }

    dynamic_array->head   = NULL;
    dynamic_array->cap    = 0;
    dynamic_array->length = 0;

    return dynamic_array;
}

int fly_array_reserve(fly_array_t *dynamic_array, int need_size)
{
    if (dynamic_array == NULL) {
    	return -1;
    }

    if (dynamic_array->cap < need_size) {
    	//need realloc memory
        int temp_cap = dynamic_array->cap? dynamic_array->cap * 2 : 8;

        if (temp_cap < need_size) 
        	temp_cap = need_size;
        
        void *temp_head = realloc(dynamic_array->head, temp_cap * sizeof(*dynamic_array->head));

        if (temp_head == NULL) {
        	printf("[ERROR] fly_array_reserve: malloc error.\n");
        	return -1;
        }

        dynamic_array->head = temp_head;
        dynamic_array->cap = temp_cap;
    }

    return 1;
}

int fly_free_array(fly_array_t *dynamic_array)
{
    if (dynamic_array == NULL) {
        printf("[ERROR] fly_free_array: para error.\n");
        return -1;
    }

    free(dynamic_array);

    dynamic_array = NULL;
    return 1;
}

