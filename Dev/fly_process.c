/********************************
operation about worker and master process.

Author: Andrew lin
********************************/
#include <signal.h>
#include <stdio.h>
#include <sys/resource.h>
#include "fly_socket.h"
#include "fly_core_file.h"

int fly_multiprocess_mode(fly_master_t *master)
{
	if (fly_master_process_init(master) == -1) {
		printf("[ERROR] fly_multiprocess_mode: master process init error.\n");
		return -1;
	}

	if (fly_master_process_cycle() == -1) {
		printf("[ERROR] fly_multiprocess_mode: master process cycle error.\n");
		return -1;
	}
}

int fly_master_process_init(fly_master_t *master)
{
	sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGIO);

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
    	printf("[WARN] fly_master_process_init: signal mask failed.\n");
    }

    sigemptyset(&set);

    //main process listen socket, so the worker process all is listening this socket
    if (fly_bind_socket_and_listen(master) == -1) {
    	printf("[ERROR] fly_master_process_init: bind listen socket error.\n");
    	return -1;
    }

    if (fly_start_worker_process(master) == -1) {
    	printf("[ERROR] fly_master_process_init: start worker process error.\n");
    	return -1;
    }

}



int fly_master_process_cycle()
{
    //todo: handle or send the signal to manage the worker process.
}

int fly_start_worker_process(fly_master_t *master)
{
	if (master == NULL) {
		printf("[ERROR] fly_start_worker_process: master is NULL.\n");
		return -1;
	}

    for (int i = 0; i < master->worker_number; ++i) {
    	fly_create_process(master, i);
    	++master->used;
    }
}

int fly_create_process(fly_master_t *master, int index)
{
	pid_t pid = fork();

	switch (pid) {
		case -1:
		    printf("[ERROR] fly_create_process: fork error.\n");
		    return -1;
		case 0:
		    //worker process
		    fly_worker_process_cycle(master, index);
		    break;
		default:
		    //master process
		    break;
	}

	return 1;
}

int fly_worker_process_init(fly_master_t *master, int index)
{
    struct rlimit rlmt;
    
    if (master->listener == NULL) {
    	printf("[ERROR] fly_worker_process_init: master's queue listener NULL.\n");
    	return -1;
    }

    //todo: temporarily we only listen one socket, so just get the queue's top ele.
    fly_listening_t *ls = fly_queue_get_top(master->listener);
    pid_t pid = getpid();
    fly_process_t *process = malloc(sizeof(fly_process_t));

    if (process == NULL) {
    	printf("[ERROR] fly_worker_process_init: malloc error.\n");
    	return -1;
    }
    
    //we use index to mark where the worker process stored
    master->process_info[index] = *process;
    process->fd = ls->fd;
    process->listener = ls;
    process->pid = pid;
    process->event_core = fly_core_init();
    process->conn_pool = fly_connection_pool_init();
    process->revent_queue = fly_init_queue();
    process->wevent_queue = fly_init_queue();

    if (process->event_core == NULL) {
    	printf("[ERROR] fly_worker_process_init: fly_core_init error.\n");
    	free(process);
    	return -1;
    } 

    if (process->conn_pool == NULL) {
    	printf("[ERROR] fly_worker_process_init: fly_connection_pool_init error.\n");
    	fly_core_clear(process->event_core);
    	free(process);
    	return -1;
    }
    
    fly_event *event = malloc(sizeof(fly_event));

    if (event == NULL) {
    	printf("[ERROR] fly_worker_process_init: malloc error.\n");
    	fly_core_clear(process->event_core);
    	fly_destroy_connection_pool(process);
    	free(process);
    	return -1;
    }

    if (fly_event_set(process->fd, fly_accept_socket, event, FLY_EVENT_READ, process, process->event_core, NULL) == -1) {
    	printf("[ERROR] fly_worker_process_init: fly_event_set error.\n");
    	fly_core_clear(process->event_core);
    	fly_destroy_connection_pool(process);
    	free(event);       
    	free(process);
        return -1;
    }

    if (fly_event_add(event) != 0) {
        printf("[ERROR] fly_worker_process_init: fly_event_add error.\n");
        fly_core_clear(process->event_core);
    	fly_destroy_connection_pool(process);
    	free(event);  
    	free(process);
    	return -1;
    }

    if (fly_insert_queue(process->revent_queue, event) == -1) {
        printf("[ERROR] fly_worker_process_init: fly_insert_queue error.\n");
        return -1;
    }

    return 1;
}

int fly_worker_process_cycle(fly_master_t *master, int index)
{
    if (fly_worker_process_init(master, index) == -1) {
    	printf("[ERROR] fly_worker_process_cycle: worker process init error.\n");
    	return -1;
    }
    
    fly_process_t process = master->process_info[index];

    for( ; ;) {
        fly_core_cycle(process.event_core);
    }
}

int fly_destroy_connection_pool(fly_process_t *process)
{
    if (process == NULL) {
        return -1;
    }

    if (process->conn_pool) {
        fly_free_array(process->conn_pool);
        return 1;
    }

    return -1;
}