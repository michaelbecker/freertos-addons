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


typedef struct MemPool_t_ {

    SemaphoreHandle_t Lock;
    
    Stack_t Stack;

    /**
     *  Save the item size for additions.
     */
    int ItemSize;

    int Alignment;

    unsigned char Buffer[1];

} MemPool_t;



MemoryPool_t *CreateMemoryPool( int itemSize, 
                                int itemCount,
                                int Alignment)
{
    /*********************************/
    MemPool_t *MemPool;
    int i;
    int realItemSize;
    int alignmentBit = 0x1;
    int alignmentCount;
    unsigned char *ptr;
    SlNode_t *Node;
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
        return NULL;
    }

    if (itemSize <= Alignment) {
        /**
         *  The 2* accounts for the SList struct inside it.
         */
        realItemSize = 2 * Alignment;
    }
    else {
        alignmentCount = itemSize / Alignment;
        if (itemSize % Alignment != 0) {
            alignmentCount++;
        }
        /**
         *  The +1 accounts for the SList struct inside it.
         */
        realItemSize = ((alignmentCount + 1) * Alignment);
    }
       
    realItemSize += sizeof(unsigned char *);

    int memPoolSize = sizeof(MemPool_t) - sizeof(unsigned char)
                    + (itemCount * realItemSize);

    MemPool = (MemPool_t *)malloc(memPoolSize);
    if (!MemPool) {
        return NULL;
    }

    MemPool->Lock = xSemaphoreCreateMutex();
    if (MemPool->Lock == NULL) {
        free(MemPool);
        return NULL;
    }

    InitStack(&MemPool->Stack);
    MemPool->ItemSize = realItemSize;
    MemPool->Alignment = Alignment;

    ptr = MemPool->Buffer;

    for (i = 0; i < itemCount; i++) {
        
        Node = (SlNode_t *)ptr;
    
        PushOnStack(&MemPool->Stack, Node);

        ptr += MemPool->ItemSize;
    }
    
    return (MemoryPool_t *)MemPool;
}


void AddMemory( MemoryPool_t *pool, 
                int itemCount)
{
    (void)pool;
    (void)itemCount;
}


void *MemoryPoolAllocate(MemoryPool_t *pool)
{
    MemPool_t *MemPool;
    SlNode_t *Node;
    unsigned char *ptr;


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
    MemPool_t *MemPool;
    SlNode_t *Node;
    unsigned char *ptr;


    MemPool = (MemPool_t *)pool;

    ptr = ((unsigned char *)memory) - MemPool->Alignment;

    Node = (SlNode_t *)ptr;

    xSemaphoreTake(MemPool->Lock, portMAX_DELAY);

    PushOnStack(&MemPool->Stack, Node);

    xSemaphoreGive(MemPool->Lock);
}



