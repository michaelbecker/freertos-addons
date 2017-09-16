

#include "FreeRTOS.h"

typedef void * MemoryPool_t;


MemoryPool_t *CreateMemoryPool( int itemSize, 
                                int itemCount);


MemoryPool_t *CreateMemoryPoolStatic(   int itemSize,
                                        void *preallocatedMemory, 
                                        int preallocatedMemorySize);


void AddMemory(int itemCount);

void AddMemory( void *preallocatedMemory, 
                int preallocatedMemorySize);


void *MemoryPoolAllocate(void);

void MemoryPoolFree(void *);


