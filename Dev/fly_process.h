/********************************
operation about worker and master process.

Author: Andrew lin
********************************/
#ifndef _FLY_PROCESS_H
#define _FLY_PROCESS_H

#define MAX_PROCESS_NUMBER 1024

#include "fly_server.h"

struct fly_process {
	pid_t              pid;
    
    //one worker process associated one core
	fly_core          *event_core;

    //one worker process associated one connection pool, free_conn is the next available fly_connection_t
    fly_connection_t  *free_conn;

    //the number of the free connection
    int                conn_number;

    //the number of the used connection
    int                used_conn_number;

    //an array which ele is the fly_connection_t
    fly_array_t       *conn_pool;

    //the conn's fd which this work process care
    //todo: now one process only care one connection, in future, need to support multi
    int                fd;
};

typedef struct fly_process fly_process_t;


int fly_multiprocess_mode(fly_master_t *master);

int fly_master_process_init(fly_master_t *master);

int fly_master_process_cycle();

int fly_start_worker_process(fly_master_t *master);

int fly_create_process(fly_master_t *master, int index);

int fly_worker_process_init(fly_master_t *master, int index);

int fly_worker_process_cycle(fly_master_t *master, int index);

#endif