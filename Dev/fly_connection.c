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

    //as the conn may reused, so need to clear the memory of the conn
    memset(conn, 0, sizeof(fly_connection_t));

    proc->free_conn = conn->next_free;
    proc->conn_number--;
    proc->used_conn_number++;

    return conn;
}

int fly_free_connection(fly_process_t *proc, fly_connection_t *conn)
{
    if (proc == NULL || conn == NULL) {
        printf("[ERROR] fly_free_connection error.\n");
        return -1;
    }

    conn->next = proc->free_conn;
    proc->free_conn = conn;
    proc->used_conn_number--;
    proc->conn_number++;

    return 1;

}