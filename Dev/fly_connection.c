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

    conn->next = proc->free_conn;
    proc->free_conn = conn;
    proc->used_conn_number--;
    proc->conn_number++;

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

    return 1;
}

int fly_read_connection(fly_connection_t *conn)
{
    if (conn == NULL) {
        return -1;
    }

    if (conn->read_buf == NULL || conn->length <= 0) {
        return -1;
    }

    int n = recv(conn->fd, conn->read_buf, conn->length, MSG_DONTWAIT);

    if (n == 0) {
    	printf("[WARN] fly_read_connection: client close the connection.\n");
    }

    if (n == -1) {
    	printf("[ERROR] fly_read_connection: recv error.\n");
    	return -1;
    }

    conn->read_buf->next = conn->read_buf->start + n;
    conn->read_buf->length -= n;

    return n;
}