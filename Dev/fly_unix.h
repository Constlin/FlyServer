/********************************
system's method's package and other thing about system.

Author: Andrew lin
********************************/
#ifndef _FLY_CONNECTION_H
#define _FLY_CONNECTION_H

#include "fly_connection.h"

/* 
   if recv return >= 0, we just return the number. 
   else if the error is EAGAIN, we return FLY_AGAIN; if EINTR, we recv again right away;
   other error we return FLY_ERROR. 

   EAGAIN means this time call is failed, maybe next time it will success
*/
int fly_recv(fly_connection_t *conn, fly_buf_t *buf, int length);

//the logic is same as fly_recv
int fly_send(fly_connection_t *conn, fly_buf_t *buf, int length);

#endif