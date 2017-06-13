/********************************
FlyServer's memory manage source file,
FlyServer use a memory pool to manage the memory(hemp memory).

Author: Andrew lin
********************************/
#include "fly_memory.h"

void *fly_malloc(int zise)
{

}

void *fly_memalign(int alignment, int size)
{

}

fly_memory_pool_t *fly_create_memory_pool(int size)
{

}

void *fly_get_memory(fly_memory_pool_t* pool, int size)
{

}

void fly_free_memory_pool(fly_memory_pool_t* pool)
{

}

int fly_memory_pool_add_cleanup(fly_memory_pool_t* pool, fly_memory_pool_cleanup_t *cleanup)
{

}