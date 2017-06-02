/********************************
operation about worker and master process.

Author: Andrew lin
********************************/
#ifndef _FLY_PROCESS_H
#define _FLY_PROCESS_H

#include <unistd.h>
#include <pthread.h>
#include "fly_core_file.h"

//used for worker process's accpeting load balance.
pthread_mutex_t fly_accept_mutex = PTHREAD_MUTEX_INITIALIZER; 

struct fly_process {
	pid_t              pid;
    
    //one worker process associated one core
	fly_core_t        *event_core;

    //one worker process associated one connection pool, free_conn is the next available fly_connection_t
    fly_connection_t  *free_conn;

    //the number of the free connection
    int                left_conn_number;

    //the number of the used connection
    int                used_conn_number;

    //an array which ele is the fly_connection_t
    fly_array_t       *conn_pool;

    //the count of the connections that the conn_pool master
    int                conn_count;

    //the conn's fd which this work process care
    //todo: now one process only care one connection, in future, need to support multi
    int                fd;
    
    //the read event that the process cares
    fly_queue_t       *revent_queue;

    //the write event that the process cares
    fly_queue_t       *wevent_queue;

    //the listener which is processed by this process
    fly_listening_t   *listener;
};

typedef struct fly_process fly_process_t;


int fly_multiprocess_mode(fly_master_t *master);

int fly_master_process_init(fly_master_t *master);

int fly_master_process_cycle(fly_master_t *master);

int fly_start_worker_process(fly_master_t *master);

//the index is the index of the array which is in fly_master_t's process_info
int fly_create_process(fly_master_t *master, int index);

int fly_worker_process_init(fly_master_t *master, int index);

int fly_worker_process_cycle(fly_master_t *master, int index);

int fly_destroy_connection_pool(fly_process_t *process);

int fly_init_process_title();

//the system store the process's title in the argv[0], so we need to change the argv[0],
int fly_set_process_title(char *title);

#endif