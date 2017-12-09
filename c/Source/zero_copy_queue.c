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
        vQueueDelete(zcq->Handle);
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


int ZcqEnqueueItem( ZeroCopyQueue_t z, 
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


void *ZcqDequeueItem(   ZeroCopyQueue_t z,
                        TickType_t Timeout)
{
    /******************************/
    PvtZeroCopyQueue_t *zcq;
    BaseType_t success;
    void *Item;
    /******************************/

    zcq = (PvtZeroCopyQueue_t *)z;

    success = xQueueReceive(zcq->Handle, &Item, Timeout);

    return success == pdTRUE ? Item : NULL;
}


