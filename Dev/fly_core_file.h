/*****************************

file that contain the core datastruct.

*****************************/

#ifndef _FLY_CORE_FILE_H
#define _FLY_CORE_FILE_H

typedef struct fly_connection  fly_connection_t;
typedef struct fly_listening   fly_listening_t;
typedef struct fly_process     fly_process_t;
typedef struct fly_event       fly_event_t;
typedef struct fly_core        fly_core_t;
typedef struct fly_master      fly_master_t;
typedef struct fly_hash        fly_hash_t;
typedef struct fly_minheap     fly_minheap_t;
typedef struct fly_queue_head  fly_queue_t;
typedef struct fly_buf         fly_buf_t;
typedef struct fly_array       fly_array_t;

#include "fly_process.h"
#include "fly_connection.h"
#include "fly_event.h"
#include "fly_server.h"
#include "fly_error.h"
#include "fly_map.h"
#include "fly_minheap.h"
#include "fly_queue.h"
#include "fly_buf.h"
#include "fly_array.h"
#include "fly_unix.h"
#include "fly_util.h"

#endif