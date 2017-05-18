#include <stdio.h>
#include <stdlib.h>
#include "fly_core_file.h"

int main()
{
	fly_master_t *master = malloc(sizeof(fly_master_t));

	if (master == NULL) {
		printf("[ERROR] main: malloc error.\n");
		return -1;
	}

	if (fly_init_master(master) == -1) {
		printf("[ERROR] main: fly_init_master error.\n");
		free(master);
		return -1;
	}

	fly_multiprocess_mode(master);
}

int fly_init_master(fly_master_t *master)
{
	if (master == NULL) {
		return -1;
	}

	master->worker_number = 4;
	master->listener = fly_init_queue();

	return 1;
}