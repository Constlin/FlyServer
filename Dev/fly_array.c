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

    if (dynamic_array->length < need_size) {
    	//need realloc memory
        int temp_length = dynamic_array->length? dynamic_array->length * 2 : 8;

        if (temp_length < need_size) 
        	temp_length = need_size;
        
        void **temp_head = realloc(dynamic_array->head, temp_length * sizeof(*(dynamic_array->head)));

        if (temp_head == NULL) {
        	printf("[ERROR] fly_array_reserve: malloc error.\n");
        	return -1;
        }

        dynamic_array->head = temp_head;
        dynamic_array->cap = dynamic_array->cap + (temp_length - dynamic_array->length);
        dynamic_array->length = temp_length;
    }

    return 1;
}

int fly_array_insert(fly_array_t *dynamic_array, void *ele)
{
    if (dynamic_array == NULL || ele == NULL) {
        return -1;
    }
    
    if (dynamic_array->cap <= 0) {
        printf("[DEBUG] fly_array_insert: the array's cap <= 0, reverse it.\n");

        if (fly_array_reserve(dynamic_array, dynamic_array->length + 1) == -1) {
            printf("[ERROR] fly_array_insert: fly_array_reserve error.\n");
            fly_free_array(dynamic_array);
            return -1;
        }
    }

    printf("[DEBUG] index: %d, length: %d, cap: %d, value: %d.\n", dynamic_array->length - dynamic_array->cap, dynamic_array->length, dynamic_array->cap, *ele);
    dynamic_array->head[dynamic_array->length - dynamic_array->cap] = ele;
    //after insert one ele successfully, we decrease the array's cap by 1.
    dynamic_array->cap -= 1;
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

/*
//test for this fly_array
int main()
{
    int i = 1; 
    int j = 2;
    int k = 3;

    fly_array_t *array = fly_array_init();
    for (int n = 0; n < 10; ++n) {
        fly_array_insert(array, &i);
    }
    
    //fly_array_insert(array, &j);
    //fly_array_insert(array, &k);
    printf("[TEST] array[0]: %d, array[1]: %d, array[2]: %d. array's length: %d, array's cap: %d.\n", (*array->head[0]), (*array->head[1]), (*array->head[2]), array->length, array->cap);

    return 0;
}
*/
