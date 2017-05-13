/********************************
operation about socket, like bind listen, etc

Author: Andrew lin
********************************/
#ifndef _FLY_SOCKET_H
#define _FLY_SOCKET_H

#include "fly_server.h"
#include "fly_connection.h"

int fly_bind_socket(const char *addr, int port);

struct addrinfo *fly_make_addr(fly_listening_t *listener);

int fly_create_worker(const char *addr, int port);

int fly_accept_socket(int fd);

int fly_bind_socket_and_listen(fly_master_t *master);

void fly_free_socket(int socket);

int fly_bin_conn_and_socket(fly_connection_t *conn, int fd);

#endif