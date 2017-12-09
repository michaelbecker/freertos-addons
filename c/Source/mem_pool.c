/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS Add-ons project.
 *
 *  Source Code:
 *  https://github.com/michaelbecker/freertos-addons
 *
 *  Project Page:
 *  http://michaelbecker.github.io/freertos-addons/
 *
 *  On-line Documentation:
 *  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************/


#include <stdlib.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "mem_pool.h"
#include "stack_simple.h"


/**
 *  The actual Memory Pool data structure.
 *
 *  This is a variable length data structure.
 */
typedef struct MemPool_t_ {

    /**
     *  We need a lock to make this thread safe.
     */
    SemaphoreHandle_t Lock;
    
    /**
     *  Memory blocks are stored on a stack.
     */
    Stack_t Stack;

    /**
     *  Save the item size for additions.
     */
    int ItemSize;

    /**
     *  The overall alignment of an item.
     */
    int Alignment;

    /**
     *  The begining of the actual memory pool itself.
     */
    unsigned char Buffer[1];

} MemPool_t;


static int CalculateAndVerifyAlignment(int Alignment)
{
    /*********************************/
    int i;
    int alignmentBit = 0x1;
    /*********************************/

    /**
     *  Guarantee that the alignment is the size of a pointer.
     */
    if (Alignment < (int)sizeof(unsigned char *)) {
        Alignment = (int)sizeof(unsigned char *);
    }

    for (i = 0; i < 31; i++) {
        if (Alignment == alignmentBit) {
            break;
        }
        alignmentBit <<= 1; 
    }

    if (i >= 31) {
        return 0;
    }
    else {
        return Alignment;
    }
}


static int CalculateItemSize(   int ItemSize,
                                int Alignment)
{
    /*********************************/
    int alignmentCount;
    /*********************************/

    if (ItemSize <= Alignment) {
        /**
         *  The 2* accounts for the SList struct inside each memory block.
         */
        ItemSize = 2 * Alignment;
    }
    else {
        alignmentCount = ItemSize / Alignment;
        if (ItemSize % Alignment != 0) {
            alignmentCount++;
        }
        /**
         *  The +1 accounts for the SList struct inside each memory block.
         */
        ItemSize = ((alignmentCount + 1) * Alignment);
    }
    
    return ItemSize;
}
    

MemoryPool_t CreateMemoryPool(  int ItemSize,
                                int ItemCount,
                                int Alignment)
{
    /*********************************/
    MemPool_t *MemPool;
    int MemPoolSize;
    unsigned char *ptr;
    SlNode_t *Node;
    int i;
    /*********************************/

    Alignment = CalculateAndVerifyAlignment(Alignment);

    if (Alignment == 0) {
        return NULL;
    }

    ItemSize = CalculateItemSize(ItemSize, Alignment);

    MemPoolSize = sizeof(MemPool_t) - sizeof(unsigned char)
                    + (ItemCount * ItemSize);

    MemPool = (MemPool_t *)malloc(MemPoolSize);
    if (!MemPool) {
        return NULL;
    }

    MemPool->Lock = xSemaphoreCreateMutex();
    if (MemPool->Lock == NULL) {
        free(MemPool);
        return NULL;
    }

    InitStack(&MemPool->Stack);
    MemPool->ItemSize = ItemSize;
    MemPool->Alignment = Alignment;

    ptr = MemPool->Buffer;

    for (i = 0; i < ItemCount; i++) {
        
        Node = (SlNode_t *)ptr;
    
        PushOnStack(&MemPool->Stack, Node);

        ptr += MemPool->ItemSize;
    }
    
    return (MemoryPool_t)MemPool;
}


int AddExtraMemoryToPool(   MemoryPool_t pool, 
                            int ItemCount)
{
    /*********************************/
    MemPool_t *MemPool;
    SlNode_t *Node;
    unsigned char *ptr;
    int i;
    int AdditionalPoolSize;
    /*********************************/

    MemPool = (MemPool_t *)pool;

    AdditionalPoolSize = ItemCount * MemPool->ItemSize;

    ptr = (unsigned char *)malloc(AdditionalPoolSize);
    if (ptr == NULL) {
        return pdFAIL;
    }

    for (i = 0; i < ItemCount; i++) {
        
        Node = (SlNode_t *)ptr;
    
        xSemaphoreTake(MemPool->Lock, portMAX_DELAY);
        
        PushOnStack(&MemPool->Stack, Node);

        xSemaphoreGive(MemPool->Lock);
    
        ptr += MemPool->ItemSize;
    }

    return pdPASS;
}


MemoryPool_t CreateMemoryPoolStatic(int ItemSize,
                                    void *PreallocatedMemory,
                                    int PreallocatedMemorySize,
                                    int Alignment)
{
    /*********************************/
    MemPool_t *MemPool;
    int MemPoolSize;
    unsigned char *ptr;
    SlNode_t *Node;
    /*********************************/

    Alignment = CalculateAndVerifyAlignment(Alignment);

    if (Alignment == 0) {
        return NULL;
    }

    ItemSize = CalculateItemSize(ItemSize, Alignment);

    MemPoolSize = sizeof(MemPool_t) - sizeof(unsigned char);

    MemPool = (MemPool_t *)malloc(MemPoolSize);
    if (!MemPool) {
        return NULL;
    }

    MemPool->Lock = xSemaphoreCreateMutex();
    if (MemPool->Lock == NULL) {
        free(MemPool);
        return NULL;
    }

    InitStack(&MemPool->Stack);
    MemPool->ItemSize = ItemSize;
    MemPool->Alignment = Alignment;

    ptr = (unsigned char *)PreallocatedMemory;

    while (PreallocatedMemorySize >= ItemSize) {
        
        Node = (SlNode_t *)ptr;
        PushOnStack(&MemPool->Stack, Node);
        ptr += MemPool->ItemSize;
        PreallocatedMemorySize -= MemPool->ItemSize;
    }
    
    return (MemoryPool_t)MemPool;
}


int AddExtraMemoryToPoolStatic( MemoryPool_t pool, 
                                void *PreallocatedMemory,
                                int PreallocatedMemorySize)
{
    /*********************************/
    MemPool_t *MemPool;
    SlNode_t *Node;
    unsigned char *ptr;
    /*********************************/

    MemPool = (MemPool_t *)pool;
    ptr = (unsigned char *)PreallocatedMemory;

    while (PreallocatedMemorySize >= MemPool->ItemSize) {

        Node = (SlNode_t *)ptr;

        xSemaphoreTake(MemPool->Lock, portMAX_DELAY);
        
        PushOnStack(&MemPool->Stack, Node);

        xSemaphoreGive(MemPool->Lock);

        ptr += MemPool->ItemSize;
    }

    return pdPASS;
}


void *MemoryPoolAllocate(MemoryPool_t pool)
{
    /*********************************/
    MemPool_t *MemPool;
    SlNode_t *Node;
    unsigned char *ptr;
    /*********************************/

    MemPool = (MemPool_t *)pool;

    xSemaphoreTake(MemPool->Lock, portMAX_DELAY);

    if (MemPool->Stack.Count == 0) {
        xSemaphoreGive(MemPool->Lock);
        return NULL;
    }

    Node = PopOffStack(&MemPool->Stack);

    xSemaphoreGive(MemPool->Lock);

    ptr = ((unsigned char *)Node) + MemPool->Alignment;

    return (void *)ptr;
}


void MemoryPoolFree(MemoryPool_t pool, void *memory)
{
    /*********************************/
    MemPool_t *MemPool;
    SlNode_t *Node;
    unsigned char *ptr;
    /*********************************/

    MemPool = (MemPool_t *)pool;

    ptr = ((unsigned char *)memory) - MemPool->Alignment;

    Node = (SlNode_t *)ptr;

    xSemaphoreTake(MemPool->Lock, portMAX_DELAY);

    PushOnStack(&MemPool->Stack, Node);

    xSemaphoreGive(MemPool->Lock);
}



