/********************************
operation about socket, like bind listen, etc

Author: Andrew lin
********************************/
#ifndef _FLY_SOCKET_H
#define _FLY_SOCKET_H

int fly_bind_socket(const char *addr, int port);

struct addrinfo *fly_make_addr(fly_listening_t *listener);

int fly_create_worker(const char *addr, int port);

int fly_accept_socket(int fd);

#endif