/********************************
operation about worker and master process.

Author: Andrew lin
********************************/
#include "fly_process.h"
#include "fly_socket.h"
#include "fly_event.h"

fly_process_t *fly_process_array[MAX_PROCESS_NUMBER];

int fly_multiprocess_mode(int worker_number)
{
	if (fly_master_process_init(worker_number) == -1) {
		printf("[ERROR] fly_multiprocess_mode: master process init error.\n");
		return -1;
	}

	if (fly_master_process_cycle() == -1) {
		printf("[ERROR] fly_multiprocess_mode: master process cycle error.\n");
		return -1;
	}
}

int fly_master_process_init(int worker_number)
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

    //todo: make this para configurable
    if (fly_bind_socket_and_listen("127.0.0.1", 80) == -1) {
    	printf("[ERROR] fly_master_process_init: bind listen socket error.\n");
    	return -1;
    }

    if (fly_start_worker_process(worker_number) == -1) {
    	printf("[ERROR] fly_master_process_init: start worker process error.\n");
    	return -1;
    }

}



int fly_master_process_cycle()
{
    //todo: handle or send the signal to manage the worker process.
}

int fly_start_worker_process(int worker_number)
{
    for (int i = 0; i < worker_number; ++i) {
    	fly_create_process();
    }
}

int fly_create_process()
{
	pid_t pid = fork();

	switch (pid) {
		case -1:
		    printf("[ERROR] fly_create_process: fork error.\n");
		    return -1;

		case 0:
		    //worker process
		    fly_worker_process_cycle();
		    break;

		default:
		    //master process
		    break;
	}

	return 1;
}

int fly_worker_process_init()
{
    //todo: fly_event init
}

int fly_worker_process_cycle()
{
    fly_worker_process_init();
}