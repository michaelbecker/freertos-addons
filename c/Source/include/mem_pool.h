
#ifndef MEM_POOL_H_
#define MEM_POOL_H_


typedef void * MemoryPool_t;


MemoryPool_t *CreateMemoryPool( int itemSize, 
                                int itemCount);


MemoryPool_t *CreateMemoryPoolStatic(   int itemSize,
                                        void *preallocatedMemory, 
                                        int preallocatedMemorySize);

// No DeleteMemoryPool() on purpose!

void AddMemory( MemoryPool_t *pool, 
                int itemCount);

void AddMemory( MemoryPool_t *pool,
                void *preallocatedMemory, 
                int preallocatedMemorySize);


void *MemoryPoolAllocate(MemoryPool_t *pool);

void MemoryPoolFree(MemoryPool_t *pool, void *memory);


#endif

