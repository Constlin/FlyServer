/********************************
operation about connection pool.

Author: Andrew lin
********************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fly_core_file.h"

//use to know which is next free connection.
int static next = -1;

//todo: fly_connection_t need to test.
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
    proc->left_conn_number = 0;
    proc->used_conn_number = 0;
    proc->conn_count = 0;

    return 1;
}

fly_connection_t *fly_get_connection(fly_process_t *proc)
{
    if (proc == NULL) {
    	return NULL;
    }

    //before get connection from conn_pool, we need to check weather there is free_conn exist,
    //if not we need to expand the conn_pool.
    if (proc->conn_count == 0) {
        //first use the conn_pool and need to expand the array.
        if (fly_array_reserve(proc->conn_pool, FLY_CONNECTION_COUNT_INIT) == -1) {
            fly_free_array(proc->conn_pool);
            printf("[DEBUG] fly_get_connection: (initialize) expand the array error.\n");
            return NULL;
        }

        //malloc memory for conn_pool->head's ele.
        for (int i = 0; i < FLY_CONNECTION_COUNT_INIT; ++i) {
        	proc->conn_pool->head[i] = malloc(sizeof(fly_connection_t));
        	if (proc->conn_pool->head[i] == NULL) {
        		//malloc error, before retun NULL we need to free the memory malloc before.
        		if (i > 0) {
        			for (int j = 0; j < i; ++j) {
        				free(proc->conn_pool->head[j]);
        			}

        			free(proc->conn_pool);
        			printf("[DEBUG] fly_get_connection: malloc error.\n");
        			return NULL;
        		}
        	}
        }

        proc->free_conn = proc->conn_pool->head[0];
        proc->left_conn_number = FLY_CONNECTION_COUNT_INIT;
        proc->used_conn_number = 0;
        proc->conn_count = FLY_CONNECTION_COUNT_INIT;
        next = 1;
    } else if (proc->free_conn == NULL) {
        //not first use conn_pool, and the free_conn is NULL need to expand the array to double.
        if (fly_array_reserve(proc->conn_pool, proc->conn_count + 1) == -1) {
            fly_free_array(proc->conn_pool);
            printf("[DEBUG] fly_get_connection: expand the array error.\n");
            return NULL;
        }

        proc->free_conn = proc->conn_pool->head[proc->conn_count];
        //notice, the conn_count is not 'proc->conn_count + 1'.
        proc->conn_count = proc->conn_pool->length;
        proc->left_conn_number = proc->conn_count - proc->used_conn_number;
        next = proc->conn_count + 1;
    } else {
        if (proc->free_conn->next_free != NULL) {
            //note: only free connection from conn_pool, the reused connection's next_free is not null.
            //in this situation, we need not to increase the 'next'.
        } else {
            ++next;
        }
        
    }
    

    //now, we can get conn from conn_pool as there are free connections exist.
    fly_connection_t *conn;
    conn = proc->free_conn;

    if (conn == NULL) {
        printf("[ERROR] fly_get_connection: conn is NULL.\n");
        return NULL;
    }

    if (conn->next_free != NULL) {
        //note: only free connection from conn_pool, the reused connection's next_free is not null.
        //in this situation, the conn_pool next free connection should be the conn->next_free.
        proc->free_conn = conn->next_free;
    } else {
        proc->free_conn = proc->conn_pool->head[next];  
    }
    //as the conn may reused, so need to clear the memory of the conn
    memset(conn, 0, sizeof(fly_connection_t));
    
    proc->left_conn_number--;
    proc->used_conn_number++;
    conn->process = proc;
    //conn->next_free = proc->free_conn;

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
    proc->left_conn_number++;

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

void fly_read_connection(int fd, fly_connection_t *conn)
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
            //
            // if revent == NULL, there may somewhere remove this revent fromo fly_core
            // incorrectly, 
            //
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
    conn->read_buf->length = conn->read_buf->length - n;
    printf("[info] fly_read_connection: conn->read_buf: %s.\n", conn->read_buf->start);
    return;
 
}
