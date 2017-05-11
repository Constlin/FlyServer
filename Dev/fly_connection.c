/********************************
operation about connection pool.

Author: Andrew lin
********************************/
#include "fly_connection.h"
#include "fly_array.h"

fly_array_t *fly_connection_pool_init()
{
    return fly_array_init();
}

fly_connection_t *fly_get_connection(fly_process_t *proc)
{
    if (proc == NULL) {
    	return -1;
    }

    fly_connection_t *conn;
    conn = proc->free_conn;

    if (conn == NULL) {
    	printf("[WARN] fly_get_connection: no free connection.\n");
    	return NULL;
    }

    proc->free_conn = conn->next_free;
    proc->conn_number--;
    proc->used_conn_number++;

    return conn;

}