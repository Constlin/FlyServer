/*****************************
opeartion about time. Only support monotonic time.

author: Andrew lin
*****************************/
#ifndef _FLY_TIME_UTIL_H
#define _FLY_TIME_UTIL_H

#include <time.h>
#include <sys/time.h>

int fly_gettime(struct timeval *tv);

void fly_time_plus(struct timeval *tv_ret, struct timeval *tv1, struct timeval *tv2);

void fly_time_sub(struct timeval *tv_ret, struct timeval *tv1, struct timeval *tv2);

int fly_comparetime(struct timeval *tv1, struct timeval *tv2);

//switch the point, make ptr1 point to ptr2's content, make ptr2 point to ptr1's content.
void fly_switch(void **ptr1, void **ptr2);

long fly_transform_tv_to_ms(struct timeval *tv);

#endif