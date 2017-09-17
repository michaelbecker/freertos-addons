

#include "FreeRTOS.h"
#include "semphr.h"
#include "mem_pool.h"



typedef struct MemPool_t_ {

    /**
     *  FreeRTOS semaphore handle.
     */
    SemaphoreHandle_t handle;
    
    /**
     *  Save the item size for additions.
     */
    const int ItemSize;

    int NumAvailable;

    unsigned char *List;

    unsigned char Buffer[1];

} MemPool_t;



MemoryPool_t *CreateMemoryPool( int itemSize, 
                                int itemCount,
                                int alignment)
{
    /*********************************/
    MemPool_t *memPool;
    unsigned char *Buffer;
    int i;
    int realItemSize;
    int alignmentBit = 0x1;
    int alignmentSlice;
    unsigned char *ptr;
    /*********************************/

    if (alignment < sizeof(unsigned char *)) {
        alignment = sizeof(unsigned char *);
    }

    for (i = 0; i < 31; i++) {
        if (alignment == alignmentBit) {
            break;
        }
        alignmentBit <<= 1; 
    }

    if (i >= 31) {
        return NULL;
    }

    if (itemSize <= alignment) {
        realItemSize = alignment + sizeof(unsigned char *);
    }
    else {
        alignmentSlice = itemSize / alignment;
        if (itemSize % alignment != 0) {
            alignmentSlice++;
        }
        realItemSize = (alignmentSlice * alignment) + sizeof(unsigned char *);
    }

    int memPoolSize = sizeof(MemPool_t) - sizeof(unsigned char)
                    + (itemCount * realItemSize);

    memPool = (MemPool_t *)malloc(memPoolSize);
    if (!memPool) {
        return NULL;
    }

    memPool->handle = xSemaphoreCreateMutex();
    if (memPool->handle == NULL) {
        free(memPool);
        return NULL;
    }

    pool->ItemSize = realItemSize;
    pool->NumAvailable = itemCount;
    memPool->List = &memPool->&Buffer[0];
    ptr = &memPool->&Buffer[0];

    for (i = 0; i < (itemCount - 1); i++) {
        *ptr = ptr + realItemSize;
        ptr += realItemSize;
    }
    *ptr = NULL;
    
    return (MemoryPool_t *)memPool;
}



MemoryPool_t *CreateMemoryPoolStatic(   int itemSize,
                                        void *preallocatedMemory, 
                                        int preallocatedMemorySize)
{
}


void AddMemory( MemoryPool_t *pool, 
                int itemCount)
{
}


void AddMemory( MemoryPool_t *pool,
                void *preallocatedMemory, 
                int preallocatedMemorySize)
{
}


void *MemoryPoolAllocate(MemoryPool_t *pool)
{
    MemPool_t *memPool = (MemPool_t *)pool;
    unsigned char *ptr;

    xSemaphoreTake(memPool->handle, portMAX_DELAY);

    if (memPool->List == NULL) {
        xSemaphoreGive(memPool->handle);
        return NULL;
    }

    ptr = memPool->List;
    memPool->List = ptr; 

    xSemaphoreGive(memPool->handle);

    

    return 
}


void MemoryPoolFree(MemoryPool_t *pool, void *memory)
{
    MemPool_t *memPool = (MemPool_t *)pool;

    xSemaphoreTake(memPool->handle, portMAX_DELAY);



    xSemaphoreGive(memPool->handle);
}



