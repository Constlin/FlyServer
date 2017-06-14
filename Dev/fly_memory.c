/********************************
FlyServer's memory manage source file,
FlyServer use a memory pool to manage the memory(hemp memory).

Author: Andrew lin
********************************/
#include "fly_memory.h"

static int fly_pagesize;

void *fly_malloc(int size)
{
	if (size <= MIN_MEMORY_POOL_SIZE) {
        size = MIN_MEMORY_POOL_SIZE;
	}

    void *p = malloc(size);

    if (p == NULL) {
    	printf("[ERROR] fly_memory: malloc error.\n");
    }

    return p;
}

void *fly_memalign(int alignment, int size)
{
	if (size <= MIN_MEMORY_POOL_SIZE) {
        size = MIN_MEMORY_POOL_SIZE;
	}

	void  *p;
    int    err;

    //get size heap memory, and align bytes by aligment.
    err = posix_memalign(&p, aligment, size);

    if (err) {
        printf("[ERROR] fly_memory: posix_memalign error.\n");
    	p = NULL;
    }

    return p;
}

fly_memory_pool_t *fly_create_memory_pool(int size)
{
    fly_memory_pool_t *pool;

    pool = fly_malloc(sizeof(fly_memory_pool_t));
    if (pool == NULL) {
    	return NULL;
    }

    fly_pagesize = getpagesize();
    //todo: check weather the MEMORY_POOL_ALIGMENT is suitable.
    pool->small_head = fly_memalign(MEMORY_POOL_ALIGMENT, size);

    if (pool->small_head == NULL) {
    	free(pool);
    	return NULL;
    }

    pool->small_head->last = pool->small_head;
    pool->small_head->end = pool->small_head + size;
    pool->small_head->next = NULL;
    pool->small_head->size = size;
    //for now we don't init the large_head until needed.
    pool->large_head = NULL;
    pool->cleanup = NULL;
    //the small memory can't bigger than a os's page size, as it will cause the cache hitted decrease.
    pool->max = (size < fly_pagesize)? size: fly_pagesize;

    return pool;
}

void *fly_get_memory(fly_memory_pool_t* pool, int size)
{
    if (pool == NULL || size < 1) {
    	return NULL;
    }

    if (size <= pool->max) {
    	//get memory from the pool's small area.
    	if (pool->small_head) {
            if (size > pool->small_head->size) {
            	//this small memory area has not enouth memory, so we get new one.
            	fly_small_memory_t *small = fly_memalign(MEMORY_POOL_ALIGMENT, pool->max);

            	if (small == NULL) {
            		return NULL;
            	}

            	small->last = small;
            	small->end = small + pool->max;
            	small->next = NULL;
            	small->size = pool->max - size;
            	pool->small_head->next = small;

            	return small->last;
            } else {
            	//current pool's small memory area has enouth memory.
            	unsigned char *last = pool->small_head->last;
            	pool->small_head->last += size;
            	pool->small_head->size -= size;

                return last;
            }
            
            return NULL;
    	} else {
    		pool->small_head = fly_memalign(MEMORY_POOL_ALIGMENT, size);
    	    pool->small_head->last = pool->small_head;
            pool->small_head->end = pool->small_head + size;
            pool->small_head->next = NULL;
            pool->small_head->size = size;

            return pool->small_head->last;
    	}  	
    } 

    //get memory from the pool's large area.
    return fly_get_large_memory(fly_memory_pool_t* pool, size);
}

void *fly_get_large_memory(fly_memory_pool_t *pool, int size)
{
    if( pool == NULL || size < 1) {
    	return NULL;
    }

    fly_large_memory_t *large;
    void *p;
    
    large = fly_malloc(sizeof(fly_large_memory_t));

    if (large == NULL) {
    	return NULL;
    }

    p = fly_malloc(size);

    if (p == NULL) {
    	free(large);
    	return NULL;
    }

    large->next = pool->large_head;
    pool->large_head = large;
    large->alloc_heap = p;

    return p;
}

void fly_free_memory_pool(fly_memory_pool_t *pool)
{
    if (pool == NULL) {
    	return;
    }

    //free small memory.
    fly_small_memory_t *current_small, *next_small;
    current_small = pool->small_head;
    next_small = NULL;

    while (current_small) {       
        next_small = current_small->next;        
        free(current_small);
        current_small = next_small;
    }

    //free large memory.
    fly_large_memory_t *current_large, *next_large;
    current_large = pool->large_head;
    next_large = NULL;

    while (current_large) {       
        next_large = current_large->next;        
        free(current_large);
        current_large = next_large;
    }

    //todo: in future, need to free the memory about cleanup data struct.

    //free the memory_pool it self.
    free(pool);

    return;
}

int fly_memory_pool_add_cleanup(fly_memory_pool_t *pool, fly_memory_pool_cleanup_t *cleanup)
{

}