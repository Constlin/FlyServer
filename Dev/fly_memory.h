/********************************
FlyServer's memory manage source file,
FlyServer use a memory pool to manage the memory(hemp memory).

FlyServer's memory pool has this features:
1.bytes align to speed up os's memory operation.
2.the small memory can't bigger than a os's page size, as it will cause the cache hitted decrease.

Author: Andrew lin
********************************/
#ifndef _FLY_MEMORY_H
#define _FLY_MEMORY_H

#define DEFAULT_MEMORY_POOL_SIZE 1024
#define MIN_MEMORY_POOL_SIZE 1024
#define MEMORY_POOL_ALIGMENT 32

typedef struct fly_small_memory fly_small_memory_t; 
typedef struct fly_large_memory fly_large_memory_t; 
typedef struct fly_memory_pool fly_memory_pool_t;
typedef struct fly_memory_pool_cleanup fly_memory_pool_cleanup_t;

typedef (void)(*fly_memory_pool_cleanup_pt)(void *data);

struct fly_small_memory {
	//the follow memory that we can use.
	unsigned char      *last;
    //the end of a fly_pool_small.
	unsigned char      *end;

	fly_small_memory_t *next;
    //the used memory's size.
	int                 size;
};

struct fly_large_memory {
	fly_large_memory_t *next;
	//point the malloc's return heap's address.
	void               *alloc_heap;
};

//for special clean
struct fly_memory_pool_cleanup {
    void                      *data;
    fly_memory_pool_cleanup_t *next;
    fly_memory_pool_cleanup_pt handle;

};

struct fly_memory_pool {
	//get small memory from small_head, it is a list that contain memory.
	fly_small_memory_t      *small_head;
	//get large memory from large_head, it is a list that contain memory.
	fly_large_memory_t      *large_head;
	//clean up operator handler for special clean when use fly_memory_pool.
	fly_memory_pool_cleanup *cleanup;
    //while getting memory from fly_pool, if user wanted memory bigger that max,
    //we get memory from large_head, else small_head.
	int                      max;
};

//use malloc to get heap memory.
void *fly_malloc(int zise);

//use posix_memalign to get heap memory.
void *fly_memalign(int alignment, int size);

fly_memory_pool_t *fly_create_memory_pool(int size);

void *fly_get_memory(fly_memory_pool_t *pool, int size);

void *fly_get_large_memory(fly_memory_pool_t *pool, int size);

void fly_free_memory_pool(fly_memory_pool_t *pool);

int fly_memory_pool_add_cleanup(fly_memory_pool_t *pool, fly_memory_pool_cleanup_t *cleanup);

#endif