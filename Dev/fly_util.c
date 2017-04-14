/*****************************
opeartion about time. Only support monotonic time.

author: Andrew lin
*****************************/
#include <stdio.h>
#include "fly_util.h"

int fly_gettime(struct timeval *tv)
{
    struct timespec ts;
    
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
    	printf("clock_gettime error.\n");
    	return -1;
    }
    
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
      
    return 0;
}

void fly_time_plus(struct timeval *tv_ret, struct timeval *tv1, struct timeval *tv2)
{
	tv_ret->tv_sec = tv1->tv_sec + tv2->tv_sec;
	tv_ret->tv_usec = tv1->tv_usec + tv2->tv_usec;

	if (tv_ret->tv_usec >= 1000000) {
		tv_ret->tv_usec -= 1000000;
		tv_ret->tv_sec  += 1;
	}

	return;
}

void fly_time_sub(struct timeval *tv_ret, struct timeval *tv1, struct timeval *tv2)
{
    //should keep the tv1 bigger than tv2.
    if (tv1->tv_usec < tv2->tv_usec) {
        --tv1->tv_sec;
        tv1->tv_usec += 1000000;
        tv_ret->tv_usec = tv1->tv_usec - tv2->tv_usec;
    } else {
        tv_ret->tv_usec = tv1->tv_usec - tv2->tv_usec;
    }

    if (tv_ret->tv_usec >= 1000000) {
        tv_ret->tv_usec -= 1000000;
        tv_ret->tv_sec  += 1;
    }

    tv_ret->tv_sec = tv1->tv_sec - tv2->tv_sec;
    
    return;
}

int fly_comparetime(struct timeval *tv1, struct timeval *tv2)
{
    if (tv1 == NULL || tv2 == NULL) {
    	printf("fly_comparetime tv1 tv2 is NULL.\n");
    	return -1;
    }
    
    if (tv1->tv_sec > tv2->tv_sec) {
    	return 1; //tv1 > tv2
    } else if (tv1->tv_sec == tv2->tv_sec) {
    	if (tv1->tv_usec == tv2->tv_usec) {
    		return 2; //tv1 == tv2
    	} else if (tv1->tv_usec < tv2->tv_usec) {
    		return 3; //tv1 < tv2
    	} else {
    		return 1; //tv1 > tv2
    	}
    } else {
    	return 3; //tv1 < tv2
    }
 
    return -1;
}

void fly_switch(void **ptr1, void **ptr2)
{   
	void *temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = temp;

	return;
}

//todo: should make sure that the bits is enough.
long fly_transform_tv_to_ms(struct timeval *tv)
{
    return tv->tv_sec * 1000 + tv->tv_usec / 1000;
}



/************************************************
    below is test code for fly_util
************************************************/
/*
void main()
{
    struct timeval tv1, tv2;
    fly_gettime(&tv1);
    fly_gettime(&tv2);
    fly_time_plus(&tv2, &tv1, &tv2);


    struct timeval *time1 = &tv1;
    struct timeval *time2 = &tv2;
    printf("tv1: %d.%d, tv2: %d.%d\n", time1->tv_sec, time1->tv_usec, time2->tv_sec, time2->tv_usec);
    //printf("the address of tv1: %d, tv2: %d\n", *tv1, *tv2);
    fly_switch(&time1, &time2);
    //printf("the address of tv1: %d, tv2: %d\n", *tv1, *tv2);
    printf("tv1: %d.%d, tv2: %d.%d\n", time1->tv_sec, time1->tv_usec, time2->tv_sec, time2->tv_usec);
    return;
} //test code for fly_util end
*/