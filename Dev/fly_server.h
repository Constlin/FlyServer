/********************************


Author: Andrew lin
********************************/
#ifndef _FLY_SERVER_H
#define _FLY_SERVER_H

#define MAX_PROCESS_NUMBER 1024

#include "fly_core_file.h"

struct fly_master {
	//the array of the listenging socket, the worker procss will only care the socket in listener
    fly_queue_t  *listener;

    //the number of worker process
    int           worker_number;

    //store the process's info
    fly_process_t process_info[MAX_PROCESS_NUMBER];

    //used of array process_info
    int           used;
};

typedef struct fly_master fly_master_t;

int fly_init_master(fly_master_t *master);

#endif