/*****************************
a map based on a array used for linux's signal.
the reason for use array is the linux's signal is not so big like in windows.
no need to care about the address conflict problem in hash table, and no hash method too.

author: Andrew lin
*****************************/
#include <stdio.h>
#include <malloc.h>
#include "fly_map.h"

fly_hash_p fly_hash_init()
{
	fly_hash_p hash;
	hash = malloc(sizeof(struct fly_hash));
	if (hash == NULL) {
		printf("[ERROR] malloc error.\n");
		return NULL;
	}

	hash->fly_sig_array = NULL;
	hash->fly_hash_cap = 0;

	return hash;
}

int fly_hash_cap(fly_hash_p hash)
{
    if (hash == NULL) {
    	printf("[ERROR] hash is NULL.\n");
    	return -1;
    }

    return hash->fly_hash_cap;
}

int fly_hash_reserve(fly_hash_p hash, int size)
{
    if (hash == NULL) {
    	printf("[ERROR] hash is NULL.\n");
    	return -1;
    }
    int before_capa = hash->fly_hash_cap;
    struct fly_sig **temp_sig_array;
    if (hash->fly_hash_cap < size) {
    	//the capacity less than the user needed, if first set capacity, we set 64 as the linux's signal
    	//sum is 64, if not, double it.
    	int temp_cap = hash->fly_hash_cap ? hash->fly_hash_cap * 2 : 64;
    	if (temp_cap < size) {
    		temp_cap = size;
    	}
    	if(!(temp_sig_array = (struct fly_sig **)realloc(hash->fly_sig_array, temp_cap * sizeof(*temp_sig_array)))) {
			printf("[ERROR] realloc error.\n");
			//todo: should free the fly_minheap
			return -1;
		}
        //set the new realloc's ele NULL.
		for (int i = before_capa; i <= size; i++) {
			temp_sig_array[i] = malloc(sizeof(struct fly_sig));
			if (temp_sig_array[i] == NULL) {
				printf("[ERROR] malloc error.\n");
				return -1;
			}
			temp_sig_array[i]->fly_queue = NULL;
			temp_sig_array[i]->fly_queue_len = 0;			
		}

		hash->fly_sig_array = temp_sig_array;
		hash->fly_hash_cap = temp_cap;
    }

    return 1;
}

int fly_hash_insert(fly_hash_p hash, struct fly_event* sig_event, int sig_num)
{
	if (hash == NULL || sig_event == NULL || sig_num < 0) {
		printf("[ERROR] fly_hash_insert error.\n");
		return -1;
	}

	if (sig_num > hash->fly_hash_cap) {
		//the hash table need to be extense.
		if (fly_hash_reserve(hash, sig_num) != 1) {
			return -1;
		}
	}

    if (hash->fly_sig_array[sig_num] != NULL) {
    	if (hash->fly_sig_array[sig_num]->fly_queue == NULL ) {
    		if ((hash->fly_sig_array[sig_num]->fly_queue = fly_init_queue()) == NULL) {
    			return -1;
    		}
    	}
    } else if (hash->fly_sig_array[sig_num] == NULL) {
    	printf("[ERROR] fly_sig_array[] is NULL.\n");
    	return -1;
    } else {
    	printf("[ERROR] fly_sig_array[] uncertained status.\n");
    	return -1;
    }

	if (fly_insert_queue((hash->fly_sig_array[sig_num])->fly_queue, sig_event) != 0) {
		return -1;
	}

	return 1;
}

