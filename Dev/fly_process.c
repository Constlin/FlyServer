/********************************
operation about worker and master process.

Author: Andrew lin
********************************/
#include "fly_process.h"
#include "fly_socket.h"
#include "fly_event.h"

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

    fly_linstening_t *ls = fly_get_top(master->listener);
    pif_t pid = getpid();

    fly_process_t *process = malloc(sizeof(fly_process_t));

    if (process == NULL) {
    	printf("[ERROR] fly_worker_process_init: malloc error.\n");
    	return -1;
    }
    
    master->process_info[index] = *process;
    process->fd = ls->fd;
    process->pid = pid;
    process->event_core = fly_core_init();
    process->conn_pool = fly_connection_pool_init();

    if (process->event_core == NULL) {
    	printf("[ERROR] fly_worker_process_init: fly_core_init error.\n");
    	free(process);
    	return -1;
    } 

    if (process->conn == NULL) {
    	printf("[ERROR] fly_worker_process_init: fly_connection_pool_init error.\n");
    	fly_core_clear(process->event_core);
    	free(process);
    	return -1;
    }

    fly_event *event = malloc(sizeof(fly_event));

    if (event == NULL) {
    	printf("[ERROR] fly_worker_process_init: malloc error.\n");
    	fly_core_clear(process->event_core);
    	fly_destroy_connection_pool();
    	free(process);
    	return -1;
    }

    if (fly_event_set(process->fd, fly_accept_socket, event, FLY_EVENT_READ, &process->fd, core, NULL) == -1) {
    	printf("[ERROR] fly_worker_process_init: fly_event_set error.\n");
    	fly_core_clear(process->event_core);
    	fly_destroy_connection_pool();
    	free(event);       
    	free(process);
        return -1;
    }

    if (fly_event_add(event) != 0) {
        printf("[ERROR] fly_worker_process_init: fly_event_add error.\n");
        fly_core_clear(process->event_core);
    	fly_destroy_connection_pool();
    	free(event);  
    	free(process);
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

    for( ; ;) {
        fly_core_cycle((master->process_info[index])->event_core);
    }
}