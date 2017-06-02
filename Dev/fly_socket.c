/********************************
operation about socket, like bind listen, etc

Author: Andrew lin
********************************/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "fly_util.h"
#include "fly_socket.h"
#include "fly_connection.h"
#include "fly_queue.h"
#include "fly_event.h"

int fly_bind_socket(fly_listening_t *listener)
{
    if (listener == NULL) {
        printf("[ERROR] fly_bind_socket: listener is null.\n");
        return -1;
    }

    if (listener->addr == NULL || listener->port < 0) {
        return -1;
    }

	struct addrinfo *ai = NULL;
    int open = 1;

    ai = fly_make_addr(listener);

    if (ai == NULL) {
    	return -1;
    }

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd == -1) {
		printf("[ERROR] fly_bind_socket: create socket error.\n");
		return -1;
	}

    listener->fd     = fd;
    listener->family = AF_INET;
    listener->type   = SOCK_STREAM;
    listener->proto  = IPPROTO_TCP;

	if (fly_set_nonblocking(fd) == -1) {
		fly_close_fd(fd);
		return -1;
	}

	if (fly_set_closeonexec(fd) == -1) {
		fly_close_fd(fd);
		return -1;
	}

    //after call close(fd), the socket fd will process TIME_WAIT and then closed.
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&open, sizeof(open)) == -1) {
    	fly_close_fd(fd);
		return -1;
    }

    if (bind(fd, ai->ai_addr, (socklen_t)ai->ai_addrlen) == -1) {   	
    	perror("[error] fly_bind_socket: bind error");
        fly_close_fd(fd);
		return -1;
    }
    
    listener->sockaddr = ai->ai_addr;
    listener->addrlen  = (socklen_t)ai->ai_addrlen;
    return 1;
}

//called when the listen socket has data
int fly_accept_socket(int fd, fly_process_t *process)
{
    if (fd != process->fd) {
        printf("[ERROR] fly_accept_socket: fd != process->fd.\n");
        return -1;
    }

    //load balance:
    //use a mutex lock, only worker process which get the mutex lock can accept a connection,
    //and only the worker process which has enough free connections will get the mutex lock.
    if ((process->used_conn_number/8 - process->left_conn_number) > 0) {
        printf("[WARN] fly_accept_socket: the worker process has two many connection need to processed, will not accept new connection.\n");
        return 0;
    }

    if (pthread_mutex_trylock(&fly_accept_mutex) != 0) {
        printf("[WARN] fly_accept_socket: try to lock the fly_accept_mutex failed.\n");
        return 0;
    }

    int accept_fd;

    if ((accept_fd = accept(process->fd, NULL, NULL)) == -1) {
        perror("[ERROR] fly_accept_socket: accept error.");
        return -1;
    }

    if (pthread_mutex_unlock(&fly_accept_mutex) != 0) {
        printf("[ERROR] fly_accept_socket: unlock the fly_accept_mutex failed.\n");
        return -1;
    }
    
    //we get a connection from current process's connection pool, and malloc memory for 
    //this connection after accpet a connection succussfully.
    fly_connection_t *conn = fly_get_connection(process);

    if (conn == NULL) {
        printf("[ERROR] fly_accept_socket: get connection error.\n");
        return -1;
    }

    fly_bind_conn_with_socket(conn, accept_fd);
    fly_bind_conn_with_listener(conn, process->listener);
    

    if (conn->read == NULL) {
        fly_free_connection(process, conn);
        printf("[ERROR] fly_accept_socket: malloc error.\n");
        return -1;
    }

    if (conn->write == NULL) {
        fly_free_connection(process, conn);
        printf("[ERROR] fly_accept_socket: malloc error.\n");
        return -1;
    }
 
    //add this connectino's read event to fly_core
    if (fly_event_set(accept_fd, fly_read_connection, conn->read, FLY_EVENT_READ, conn, process->event_core, NULL) == -1) {
        fly_free_connection(process, conn);
        printf("[ERROR] fly_accept_socket: fly_event_set error.\n");
        return -1;
    }

    if (fly_event_add(conn->read) != 0) {
        fly_free_connection(process, conn);
        printf("[ERROR] fly_accept_socket: fly_event_add error.\n");
        return -1;
    }

    if (fly_insert_queue(process->revent_queue, conn->read) == -1) {
        fly_free_connection(process, conn);
        printf("[ERROR] fly_accept_socket: fly_insert_queue error.\n");
        return -1;
    }

    //add this connection's write event to fly_core, the connection's write event should be unpersist event,
    //so this write event will only be oneshot,.
    if (fly_event_set(accept_fd, fly_write_connection, conn->write, FLY_EVENT_WRITE | FlY_EVENT_UNPERSIST, conn, process->event_core, NULL) == -1) {
        fly_free_connection(process, conn);
        printf("[ERROR] fly_accept_socket: fly_event_set error.\n");
        return -1;
    }

    //we should add this event to fly_core again while we need.
    /*
    if (fly_event_add(wevent) != 0) {
        fly_free_connection(process, conn);
        free(revent);
        free(wevent);
        printf("[ERROR] fly_accept_socket: fly_event_add error.\n");
        return -1;
    }
    

    if (fly_insert_queue(process->wevent_queue, wevent) == -1) {
        fly_free_connection(process, conn);
        free(revent);
        free(wevent);
        printf("[ERROR] fly_accept_socket: fly_insert_queue error.\n");
        return -1;
    }
    */
    
    printf("[INFO] fly_accept_socket: accept a connection successfully. listen fd: %d, accept_fd: %d.\n", fd, accept_fd);
    return 1;

    //add the read event for this connection to fly_core
}

int fly_bind_socket_and_listen(fly_master_t *master)
{
    fly_listening_t *listener = malloc(sizeof(fly_listening_t));

    if (listener == NULL) {
        printf("[ERROR] fly_bind_socket_and_listen: malloc error.\n");
        return -1;
    }

    listener->addr = FLY_LOCAL_ADDRESS;
    listener->port = FLY_LOCAL_PORT;

	if (fly_bind_socket(listener) != 1) {
        printf("[ERROR] fly_bind_socket_and_listen: fly_bind_socket error.\n");
        free(listener);
        return -1;
    }

    int fd = listener->fd;

	if (fd == -1) {
		printf("[ERROR] fly_bind_socket_and_listen: fd is -1.\n");
        free(listener);
		return -1;
	}

    //set the kernal's queue's length which used for listening  to 128
    if (listen(fd, 128) == -1) {
    	printf("[ERROR] fly_bind_socket_and_listen: listen socket error.\n");
    	fly_close_fd(fd);
        free(listener);
		return -1;
    }

    //after listen fd, we don't add this to fly_core right away, we do this in worker process

    if (master->listener == NULL) {
    	printf("[ERROR] fly_bind_socket_and_listen: queue listener is NULL.\n");
    	return -1;
    }

    if (fly_insert_queue(master->listener, listener) == -1) {
    	printf("[ERROR] fly_bind_socket_and_listen: insert queue listener error.\n");
    	return -1;
    }

    printf("[INFO] fly_bind_socket_and_listen successfully, addr: %s, port: %d\n", listener->addr, listener->port);

    return 1;
}

struct addrinfo *fly_make_addr(fly_listening_t *listener)
{
    if (listener == NULL) {
        printf("[ERROR] fly_make_addr: listener is null.\n");
        return NULL;
    }

    if (listener->addr == NULL || listener->port < 0) {
        printf("[ERROR] fly_make_addr: listener has null.\n");
        return NULL;
    }

    struct addrinfo hints;
    struct addrinfo *ai = NULL;
    char strport[32];

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    fly_int_to_char(listener->port, strport);

    if (getaddrinfo(listener->addr, strport, &hints, &ai) != 0) {
    	return NULL;
    }

    return ai;
}

void fly_free_socket(int socket) 
{
    close(socket);

    return;
}

int fly_bind_conn_with_socket(fly_connection_t *conn, int fd)
{
    if (conn == NULL || fd < 0) {
        return -1;
    }

    conn->fd = fd;
    return 1;
}

int fly_bind_conn_with_listener(fly_connection_t *conn, fly_listening_t *listener)
{
    if (conn == NULL || listener == NULL) {
        return -1;
    }

    conn->listener = listener;
    return 1;
}

