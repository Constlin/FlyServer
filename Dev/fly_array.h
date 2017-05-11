/********************************
operation about dynamic array

Author: Andrew lin
********************************/
#ifndef _FLY_ARRAY_H
#define _FLY_ARRAY_H

struct fly_array {
    void *head;

    int   cap;

    int   length;
};

typedef struct fly_array fly_array_t;

fly_array_t *fly_array_init();

int fly_array_reserve();

#endif