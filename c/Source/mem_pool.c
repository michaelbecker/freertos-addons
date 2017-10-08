/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C Add-ons project.
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
 *  The FreeRTOS C Add-ons project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The FreeRTOS C Add-ons project is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the FreeRTOS C Add-ons project.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C Add-ons project
 *  without being obliged to provide the source
 *  code for proprietary components.
 * 
 *  The FreeRTOS C Add-ons project GPL exception text follows:
 * 
 *  Any FreeRTOS C Add-ons *source code*, whether modified or in it's 
 *  original release form, or whether in whole or in part, can only be 
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C Add-ons project.
 * 
 *  Clause 1:
 * 
 *  Linking FreeRTOS C Add-ons project  with other modules is making a 
 *  combined work based on FreeRTOS C Add-ons project. Thus, the terms 
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 * 
 *  As a special exception, the copyright holders of FreeRTOS C Add-ons
 *  project give you permission to link FreeRTOS C Add-ons project with 
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and 
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the 
 *  terms and conditions of the license of that module.  An independent 
 *  module is a module which is not derived from or based on FreeRTOS C 
 *  Add-ons project.
 * 
 *  Clause 2:
 * 
 *  FreeRTOS C Add-ons project may not be used for any competitive or 
 *  comparative purpose, including the publication of any form of run time 
 *  or compile time metric, without the express permission of the copyright 
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
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
    

MemoryPool_t *CreateMemoryPool( int ItemSize,
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
    
    return (MemoryPool_t *)MemPool;
}


int AddExtraMemoryToPool(   MemoryPool_t *pool, 
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


MemoryPool_t *CreateMemoryPoolStatic(   int ItemSize,
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
    
    return (MemoryPool_t *)MemPool;
}


int AddExtraMemoryToPoolStatic( MemoryPool_t *pool, 
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


void *MemoryPoolAllocate(MemoryPool_t *pool)
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


void MemoryPoolFree(MemoryPool_t *pool, void *memory)
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



