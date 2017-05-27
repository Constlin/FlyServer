/********************************
system's method's package and other thing about system.

Author: Andrew lin
********************************/
#include <sys/stat.h> 
#include "fly_unix.h"

int fly_recv(fly_connection_t *conn, fly_buf_t *buf, int length)
{
	if (conn == NULL || buf == NULL || length < 0) {
		return FLY_ERROR;
	}

    int err;
    int n;
    
	do {
        //we use recv to copy the data from kernal's tcpip stack's buffer to user's buffer.
        //every recv will return the data that exist in the kernal's buffer. if recv data's number > 0, we return right away.
        n = recv(conn->fd, buf->next, length, 0);

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
        //the send() just move the data from user space to kernal space, for the time that data sent to remote server, 
        //it depends on TCP/IP stack.
        n = send(conn->fd, buf->start, length, 0);

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

int fly_read(int fd, fly_buf_t *buf, int length)
{
    if (fd < 0 || buf == NULL || length <= 0) {
        return -1;
    }

    int leftbytes = length;
    int n;
    char *p = buf->next;

    while (leftbytes > 0) {
        n = read(fd, p, length);

        if (n == 0) 
            //n == 0 means that we reach the file's tail before we read length bytes.
            return 0;

        if (n < 0) {
            if (errno == EINTR) {
                n = 0;
            } else {
                return -1;
            }
        }

        leftbytes -= n;
        p += n;
    }

    return length - leftbytes;
}

int fly_write(int fd, fly_buf_t *buf, int length)
{

}

unsigned long fly_get_file_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  

    if (stat(path, &statbuff) < 0) {  
        return filesize;  
    } else {  
        filesize = statbuff.st_size;  
    }  

    return filesize;  
}