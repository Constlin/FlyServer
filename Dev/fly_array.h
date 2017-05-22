/********************************
operation about dynamic array

how to use fly_array:
1).fly_array_init(), notice in fly_array_init() we just return a struct(fly_array_t)'s pointer
2).use fly_array_insert to insert ele to the array, just no need to care the space is enough or
not enough, we process it automatically.

Author: Andrew lin
********************************/
#ifndef _FLY_ARRAY_H
#define _FLY_ARRAY_H

struct fly_array {
    void **head;
    
    //the cap menas that how many elements that the array can store now.
    int   cap;

    //the length means array's length.
    int   length;
};

typedef struct fly_array fly_array_t;

fly_array_t *fly_array_init();

int fly_array_reserve(fly_array_t *dynamic_array, int need_size);

//insert ele to the end of a dynamic_array, if cap is enough we just insert at next free space; if not enough, we reserve it,
//init value is 8, after it double it.
int fly_array_insert(fly_array_t *dynamic_array, void *ele);

int fly_free_array(fly_array_t *dynamic_array);

#endif