/********************************
operation about a buffer, which is 
used for read/write socket

Author: Andrew lin
********************************/
#include <malloc.h>
#include "fly_buf.h"
#include 

fly_buf_t *fly_init_buf(int bytes)
{
    fly_buf_t *buf = malloc(sizeof(fly_buf_t));

    if (buf == NULL) {
    	return NULL;
    }

    buf->start = malloc(sizeof(bytes));

    if (buf->start == NULL) {
    	free(buf);
    	return NULL;
    }

    buf->next = start;
    buf->end = start + bytes;
    buf->length = bytes;

    return buf;
}

int fly_free_buf(fly_buf_t *buf)
{
	if (buf->start && buf  != NULL) {
		free(buf->start);
		free(buf);
		return 1;
	}

	if (buf != NULL) {
		free(buf);
		return 1;
	}

	return -1;
}