/********************************
operation about connection pool.

Author: Andrew lin
********************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fly_core_file.h"

int fly_connection_pool_init(fly_process_t *proc)
{
    if (proc == NULL) {
        return -1;
    }

    proc->conn_pool = fly_array_init();

    if (proc->conn_pool == NULL) {
        return -1;
    }

    proc->free_conn = NULL;
    proc->conn_number = 0;
    proc->used_conn_number = 0;
    proc->conn_count = 0;

    //first expand.
    if (fly_expand_connection_pool(proc) == -1) {
        return -1;
    }

    return 1;
}

fly_connection_t *fly_get_connection(fly_process_t *proc)
{
    if (proc == NULL) {
    	return NULL;
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
    conn->process = proc;

    if (fly_init_connection(conn) == -1) {
        printf("[ERROR] fly_get_connection: fly_init_connection error.\n");
        return NULL;
    }

    return conn;
}

int fly_free_connection(fly_process_t *proc, fly_connection_t *conn)
{
    if (proc == NULL || conn == NULL) {
        printf("[ERROR] fly_free_connection error.\n");
        return -1;
    }

    conn->next_free = proc->free_conn;
    proc->free_conn = conn;
    proc->used_conn_number--;
    proc->conn_number++;

    if (conn->read_buf) {
    	free(conn->read_buf);
    }

    if (conn->write_buf) {
    	free(conn->write_buf);
    }

    if (conn->read) {
    	free(conn->read);
    }

    if (conn->write) {
    	free(conn->write);
    }

    fly_close_fd(conn->fd);

    return 1;
}

int fly_init_connection(fly_connection_t *conn)
{
    if (conn == NULL) {
        return -1;
    }

    if (conn->read_buf == NULL) {
        conn->read_buf = fly_init_buf(CONNECTION_READ_BUFFER);
        if (conn->read_buf == NULL) {
            printf("[ERROR] fly_init_connection: fly_init_buf error.\n");
            return -1;
        }
    }

    if (conn->write_buf == NULL) {
        conn->write_buf = fly_init_buf(CONNECTION_WRITE_BUFFER);
        if (conn->write_buf == NULL) {
            fly_free_buf(conn->read_buf);
            printf("[ERROR] fly_init_connection: fly_init_buf error.\n");
            return -1;
        }
    }
 
    if (conn->read == NULL) {
    	conn->read = malloc(sizeof(fly_event_t));
    	if (conn->read == NULL) {
    		fly_free_buf(conn->read_buf);
    		fly_free_buf(conn->write_buf);
    		return -1;
    	}
    }

    if (conn->write == NULL) {
    	conn->write = malloc(sizeof(fly_event_t));
    	if (conn->write == NULL) {
    		fly_free_buf(conn->read_buf);
    		fly_free_buf(conn->write_buf);
    		free(conn->write);
    		return -1;
    	}
    }

    return 1;
}

void fly_read_connection(fly_connection_t *conn)
{
    if (conn == NULL) {
        return;
    }

    if (conn->read_buf == NULL || conn->read_buf->length <= 0) {
        return;
    }

    int n = fly_recv(conn, conn->read_buf, conn->read_buf->length);

    if (n == 0) {
    	printf("[WARN] fly_read_connection: client close the connection.\n");
    	return;
    }

    if (n == FLY_ERROR) {
    	printf("[ERROR] fly_read_connection: recv error.\n");
    	return;
    }

    if (n == FLY_AGAIN) {
    	//add this read event to fly_core again
    	//todo: find this revent in process's revent_queue
    	fly_event_t *revent = fly_use_fd_find_event(conn->fd, conn->process->revent_queue);

    	if (revent == NULL) {
            printf("[ERROR] fly_read_connection: fly_use_fd_find_event return NULL.\n");
            /*
              if revent == NULL, there may somewhere remove this revent fromo fly_core
              incorrectly, 
            */
            fly_free_connection(conn->process, conn);
            return -1;
    	}

    	if (fly_event_set(conn->fd, fly_read_connection, revent, FLY_EVENT_READ, NULL, conn->process->event_core, NULL) == -1) {
            fly_free_connection(conn->process, conn);
            free(revent);
            printf("[ERROR] fly_read_connection: fly_event_set error.\n");
            return;
        }
    }

    conn->read_buf->next = conn->read_buf->start + n;
    conn->read_buf->length -= n;

    return;
}

int fly_connection_count(fly_process_t *proc)
{
    if (proc == NULL) {
        return -1;
    }

    return proc->conn_count;
}

int fly_expand_connection_pool(fly_process_t *proc)
{
    if (proc == NULL) {
        return -1;
    }

    if (proc->conn_number > 0) {
        //free connection > 0, so no need to expand.
        return 0;
    }

    if (proc->conn_count == 0) {
        //first expand connection pool, we make the connection pool's connection count is 100.
        if (fly_array_reserve(proc->conn_pool, FLY_CONNECTION_COUNT_INIT) == -1) {
            return -1;
        } else {
            proc->free_conn = proc->conn_pool->head;
            proc->conn_number = proc->conn_pool->
            return 1;
        }
    }

    if (proc->conn_count != 0 && proc->conn_number == 0) {
        //the connection pool has no free connection, need to expand to be double.
        if (fly_array_reserve(proc->conn_pool, proc->conn_count + 1) == -1) {
            return -1;
        } else {
            return 1;
        }
    }

    //unknown error.
    return -1;
}