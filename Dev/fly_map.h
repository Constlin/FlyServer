/*****************************
a map based on a array used for linux's signal.
the reason for use array is the linux's signal is not so big like in windows.
no need to care about the address conflict problem in hash table, and no hash method too.

author: Andrew lin
*****************************/
#ifndef _FLY_MAP_H
#define _FLY_MAP_H

#include "fly_queue.h"
#include "fly_event.h"

struct fly_hash {
	//fly_sig is an array, the array's ele is an pionter which point to fly_sig,
    //the index is the signal's number, the fly_sig_array[0] means signal which number is 1.
    void **fly_sig_array;         
    //the capacity of the array.
    int    fly_hash_cap;    
};

struct fly_sig {
	//a queue's head, which ele is the fly_event.
    struct fly_queue_head *fly_queue; 

    int                    fly_queue_len;
};

typedef struct fly_hash* fly_hash_p;

fly_hash_p fly_hash_init();

int fly_hash_cap(fly_hash_p hash);

int fly_hash_reserve(fly_hash_p hash, int size);

int fly_hash_insert(fly_hash_p hash, struct fly_event* sig_event, int sig_num);



#endif