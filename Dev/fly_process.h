/********************************
operation about worker and master process.

Author: Andrew lin
********************************/
#ifndef _FLY_PROCESS_H
#define _FLY_PROCESS_H

#define MAX_PROCESS_NUMBER 1024

struct fly_process {
	pid_t             pid;
    
    //one worker process associated one core
	fly_core         *core;

    //one worker process associated one connection pool
    fly_connection_t *conn;
};

typedef struct fly_process fly_process_t;


int fly_multiprocess_mode(int worker_number);

int fly_master_process_init(int worker_number);

int fly_master_process_cycle();

int fly_start_worker_process(int worker_number);

int fly_create_process();

int fly_worker_process_init();

int fly_worker_process_cycle();

#endif