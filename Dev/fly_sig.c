/*****************************
source code about adding signal to fly_core

author: Andrew lin
*****************************/
#include "fly_sig.h"
#include "fly_event.h"
#include "fly_map.h"

static int evsig_fd = -1;
static struct fly_hash *hash = NULL;

int fly_sig_init(fly_core *core)
{
    //call method socketpair. set the fd to close-on-exec and non-blocking.
    if (fly_make_sockepair(AF_UNIX, SOCK_STREAM, 0, core->fly_socketpair) != 0) {
        return -1;
    }

    if (fly_event_set(core->fly_socketpair[0], fly_evsig_cb, &core->fly_evsig, FLY_EVENT_INTERNAL | FLY_EVENT_READ, NULL, core, NULL) != 0) {
        return -1;
    }

    evsig_fd = core->fly_socketpair[1];
    hash = core->fly_hash;

    return 0;
}

int fly_evsig_cb(int fd, void *arg)
{
    //read the fly_socketpair[0] to get the signal number.
    int signal = 0;
    int ret = 0;
    if (read(fd, &signal, 1) != 1) {
        printf("[ERROR] read socketpair error.\n");
        return -1;
    }

    //get user set signal event.
    //solution: add a map in fly_core to store the sig -- event pair, then we can use signal to get the user event.
    struct fly_queue_head *queue = fly_get_queue_by_signal(hash, signal);
    if (queue == NULL) {
        printf("[ERROR] fly_get_queue_by_signal error.\n");
        return -1;
    }
  
    //and then add all signal events at queue to active queue.
    struct fly_event* sig_event;
    while (fly_queue_empty(queue) != 1) {
        sig_event = fly_pop_queue(queue);
        if (sig_event == NULL) {
            printf("[ERROR] signal event get from hash is NULL.\n");
            return -1;
        }

        ret = fly_insert_queue(sig_event->core->fly_active_queue, sig_event);
        if (ret == -1) {
            printf("[ERROR] signal event insert to active queue error.\n");
            return -1;
        }
    }

    return 1;

}

//note that if use signal method to set signal handler, this will only take effect ont time,
//signal called again in the handler method or use sigaction can avoid it.
int fly_set_sig_handler(int sig, void (*sig_handler)(int i))
{
    if (sig < 0) {
        return -1;
    }

    if (signal(sig, sig_handler) == SIG_ERR) {
        return -1;
    }

    return 0;
}

int fly_sig_handler(int i)
{
    if (evsig_fd == -1) {
    	return -1;
    }

    unsigned char message;
    message = i;
    int ret = write(evsig_fd, &message, sizeof(char));
    if (ret != 1) {
    	return -1;
    }
    return 0;
}

struct fly_queue_head *fly_get_queue_by_signal(struct fly_hash *hash, int signal)
{
    if (signal < 0 || hash == NULL) {
        printf("[ERROR] fly_get_queue_by_signal paras error.\n");
        return -1;
    }

    if (hash->fly_sig_array != NULL) {
        if (fly_hash_cap(hash) < signal) {
            printf("[ERROR] fly_get_queue_from_signal fly_hash's cap less than signal.\n");
            return -1;
        }

        return (hash->fly_sig_array[signal])->fly_queue;
    }

    return -1;
}






