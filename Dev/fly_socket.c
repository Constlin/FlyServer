/********************************
operation about socket, like bind listen, etc

Author: Andrew lin
********************************/
#include "fly_util.h"
#include "fly_socket.h"
#include "fly_connection.h"
#include "fly_queue.h"

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

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_CP);

	if (fd == -1) {
		printf("[ERROR] create socket error.\n");
		return -1;
	}

    listener->fd     = fd;
    listener->family = AF_INET;
    listener->type   = SOCK_STREAM;
    listener->proto  = IPPROTO_CP;

	if (fly_set_nonblocking(fd) == -1) {
		fly_close_fd(fd);
		return -1;
	}

	if (fly_set_closeonexec(fd) == -1) {
		fly_close_fd(fd);
		return -1;
	}

    //after call close(fd), the socket fd will process TIME_WAIT and then closed.
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on)) == -1) {
    	fly_close_fd(fd);
		return -1;
    }

    if (bind(fd, ai->ai_addr, (socketlen_t)ai->ai_addrlen) == -1) {
    	printf("[ERROR] bin socket error.\n");
    	fly_close_fd(fd);
		return -1;
    }

    listener->sockaddr = ai->ai_addr;
    listener->addrlen  = (socketlen_t)ai->ai_addrlen;

    return 1;
}

//todo: complete the logic about processing tcp connection coming and next opearation about read and write
int fly_accept_socket(struct fly_process_t *process)
{
    int fd;

    if ((fd = accept(process->fd, process->listen->sockaddr, process->listen->addrlen)) == -1) {
        printf("[ERROR] accept error.\n");
        return -1;
    }

    fly_connection_t *conn = fly_get_connection(process);

    if (conn == NULL) {
        printf("[ERROR] fly_accept_socket: get connection error.\n");
        return -1;
    }

    fly_bin_conn_and_socket(conn, fd);
    
    //todo: implete this method
    fly_prepare_after_accept()

    fly_event_t *revent = malloc(sizeof(fly_event_t));

    if (revent == NULL) {
        fly_free_connection(process, conn);
        printf("[ERROR] fly_accept_socket: malloc error.\n");
        return -1;
    }

    if (fly_event_set(fd, fly_read_connection, &revent, FLY_EVENT_READ, NULL, process->event_core, NULL) == -1) {
        fly_free_connection(process, conn);
        free(revent);
        printf("[ERROR] fly_accept_socket: fly_event_set error.\n");
        return -1;
    }

    if (fly_event_add(revent) != 0) {
        fly_free_connection(process, conn);
        free(revent);
        printf("[ERROR] fly_accept_socket: fly_event_add error.\n");
        return -1;
    }

    if (fly_insert_queue(process->revent_queue, revent) == -1) {
        printf("[ERROR] fly_accept_socket: fly_insert_queue error.\n");
        return -1;
    }

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

    listener->addr = addr;
    listener->port = port;

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

    if (master->listener == NULL) {
    	printf("[ERROR] fly_bind_socket_and_listen: queue listener is NULL.\n");
    	return -1;
    }

    if (fly_insert_queue(master->listener, listener) == -1) {
    	printf("[ERROR] fly_bind_socket_and_listen: insert queue listener error.\n");
    	return -1;
    }

    printf("[DEBUG] fly_bind_socket_and_listen successfully, addr: %s, port: %d", listener->addr, listener->port)

    return 1;
}

struct addrinfo *fly_make_addr(fly_listening_t *listener)
{
    if (listener == NULL) {
        printf("[ERROR] fly_make_addr: listener is null.\n");
        return -1;
    }

    if (listener->addr == NULL || listener->port < 0) {
        printf("[ERROR] fly_make_addr: listener has null.\n");
        return -1;
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
    	return -1;
    }

    return ai;
}

void fly_free_socket(int socket) 
{
    close(socket);

    return;
}

int fly_bin_conn_and_socket(fly_connection_t *conn, int fd)
{
    if (conn == NULL || fd < 0) {
        return -1;
    }

    conn->fd = fd;
    return 1;
}

