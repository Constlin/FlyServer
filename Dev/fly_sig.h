/*****************************
source code about adding signal to fly_core

author: Andrew lin
*****************************/
#ifndef _FLY_SIG_H
#define _FLY_SIG_H

int fly_sig_init(fly_core *core);

//add signal event to fly_active_queue
int fly_evsig_cb(int fd, void *arg);

int fly_set_sig_handler(int sig, void (*sig_handler)(int i));

//signal handler method, write signal data to socketpair[1]
int fly_sig_handler(int i);

//use signal to get the attached events' queue from  fly_hash.
struct fly_queue_head *fly_get_queue_by_signal(struct fly_hash *hash, int signal);

#endif