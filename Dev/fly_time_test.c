/*****************************
Below is test code for time event.

author: Andrew Lin
*****************************/
#include <stdio.h>
#include <time.h>
#include "fly_event.h"

void time_out()
{
    printf("the time is out.\n");
}


void main()
{
    fly_event event;

    fly_core *core = fly_core_init();
    if (core == NULL) {
        printf("fly_core_init error.\n");
        return;
    }
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    //set the timeout event will happen after 5 seconds.
    if (fly_event_set(-1, time_out, &event, FLY_EVENT_READ, NULL, core, &tv) == -1) {
        printf("fly_event_set error.\n");
        return;
    }

    fly_event_add(&event);

    fly_core_cycle(core);
    return;
}
