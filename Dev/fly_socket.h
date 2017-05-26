/********************************
operation about socket, like bind listen, etc

Author: Andrew lin
********************************/
#ifndef _FLY_SOCKET_H
#define _FLY_SOCKET_H

#define FLY_LOCAL_ADDRESS "10.40.30.81"
#define FLY_LOCAL_PORT     80

#include <sys/socket.h>
#include <netdb.h>
#include "fly_server.h"
#include "fly_connection.h"
#include "fly_process.h"

int fly_bind_socket(fly_listening_t *listener);

struct addrinfo *fly_make_addr(fly_listening_t *listener);

int fly_create_worker(const char *addr, int port);

//todo: should init a connection's struct after accept a socket successfully.
int fly_accept_socket(int fd, fly_process_t *process);

int fly_bind_socket_and_listen(fly_master_t *master);

void fly_free_socket(int socket);

int fly_bind_conn_with_socket(fly_connection_t *conn, int fd);

int fly_bind_conn_with_listener(fly_connection_t *conn, fly_listening_t *listener);

#endif