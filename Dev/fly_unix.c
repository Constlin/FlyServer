/********************************
system's method's package and other thing about system.

Author: Andrew lin
********************************/
#include "fly_unix.h"

int fly_recv(fly_connection_t *conn, fly_buf_t *buf, int length)
{
	if (conn == NULL || buf == NULL || length < 0) {
		return FLY_ERROR;
	}

    int err;
    int n;

	do {
        n = recv(conn->fd, buf, length, 0);

        if (n == 0) {
        	return n;
        } 

        if (n > 0) {
        	return n;
        }

        //the global error.
        err = errno;

        if (err == EAGAIN || err == EINTR) {
        	n = FLY_AGAIN;
        } else {
        	n = FLY_ERROR;
        	break;
        }

	} while(err == EINTR);

	return n;
}

int fly_send(fly_connection_t *conn, fly_buf_t *buf, int length)
{
    if (conn == NULL || buf == NULL || length < 0) {
        return FLY_ERROR;
    }

    int err;
	int n;

	for ( ;; ) {
        n = send(conn->fd, buf, length, 0);

        if (n == 0) {
        	return n;
        }

        if (n > 0) {
        	return n;
        }

        err = errno;

        if (err == EAGAIN || err == EINTR) {
        	if (err == EAGAIN) {
        		return EAGAIN;
        	}
        	//if err == EINTR, just ignore and send again right away
        } else {
        	return FLY_ERROR;
        }      
	}
}