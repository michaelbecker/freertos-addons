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
 ***************************************************************************/
#include "FreeRTOS.h"
#include "queue.h"
#include "mem_pool.h"
#include "zero_copy_queue.h"


typedef struct PvtZeroCopyQueue_t_ {

    MemoryPool_t Pool;
    QueueHandle_t Handle;

} PvtZeroCopyQueue_t;


ZeroCopyQueue_t ZcqCreateQueue( int ItemSize,
                                int ItemCount,
                                int Alignment)
{
    /******************************/
    PvtZeroCopyQueue_t *zcq;
    /******************************/

    zcq = (PvtZeroCopyQueue_t *)malloc(sizeof(PvtZeroCopyQueue_t));
    if (zcq == NULL) {
        return NULL;
    }
    
    zcq->Handle = xQueueCreate(ItemCount, sizeof(void *));
    if (zcq->Handle == NULL) {
        free(zcq);
        return NULL;
    }

    zcq->Pool = CreateMemoryPool(ItemSize, ItemCount, Alignment);
    if (zcq->Pool == NULL) {
        free(zcq);
        vQueueDelete(handle);
        return NULL;
    }

    return (ZeroCopyQueue_t)zcq;
}


void *ZcqAllocateItem(ZeroCopyQueue_t z)
{
    /******************************/
    PvtZeroCopyQueue_t *zcq;
    void *Item;
    /******************************/

    zcq = (PvtZeroCopyQueue_t *)z;

    Item = MemoryPoolAllocate(zcq->Pool);

    return Item;
}


void ZcqFreeItem(   ZeroCopyQueue_t z, 
                    void *Item)
{
    /******************************/
    PvtZeroCopyQueue_t *zcq;
    /******************************/

    zcq = (PvtZeroCopyQueue_t *)z;

    MemoryPoolFree(zcq->Pool, Item);
}


int ZcqEnqueueItem( ZeroCopyQueue_t zcq, 
                    void *Item, 
                    TickType_t Timeout)
{
    /******************************/
    PvtZeroCopyQueue_t *zcq;
    BaseType_t success;
    /******************************/

    zcq = (PvtZeroCopyQueue_t *)z;

    success = xQueueSendToBack(zcq->Handle, &Item, Timeout);

    return success == pdTRUE ? 1 : 0;
}


void *ZcqDequeueItem(   ZeroCopyQueue_t zcq,
                        TickType_t Timeout)
{
    /******************************/
    PvtZeroCopyQueue_t *zcq;
    BaseType_t success;
    void *Item;
    /******************************/

    zcq = (PvtZeroCopyQueue_t *)z;

    success = xQueueReceive(zcq->Handle, Item, Timeout);

    return success == pdTRUE ? Item : NULL;
}


