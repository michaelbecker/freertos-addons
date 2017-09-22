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
#include "workqueue.h"
#include "queue_simple.h"


/**
 *  Work Item, what we actually use internally.
 */
typedef struct pvtWorkItem_t_ {

    /**
     *  How we link into the WorkQueue.
     */
    DlNode_t Node;

    /**
     *  The actual function pointer.
     */
    WorkItem_t Function;

    /**
     *  User supplier data.
     */
    void *UserData;

} pvtWorkItem_t;


typedef struct pvtWorkQueue_t_ {

    Queue_t Queue;

    /**
     *  FreeRTOS semaphore handle.
     */
    SemaphoreHandle_t ThreadComplete;

    SemaphoreHandle_t QueueHasData;

    /**
     *  Mutex protection
     */
    SemaphoreHandle_t Lock;

    /**
     *  Reference to the underlying task handle for this thread.
     *  Can be obtained from GetHandle().
     */
    TaskHandle_t WorkerThread;

} pvtWorkQueue_t;



static void WqWorkerThread(void *parameters)
{
    /***************************/
    pvtWorkQueue_t *wq;
    DlNode_t *Node;
    pvtWorkItem_t *wi;
    /***************************/


    wq = (pvtWorkQueue_t*)parameters;


    while (true) {

        xSemaphoreTake(WorkQueue->QueueHasData);

        /**
         *  Lock the queue
         */
        xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);

        while ( !IsQueueEmpty(&wq->Queue)) {
            /**
             *  
             */
            Node = Dequeue(&WorkQueue->Queue);

            /**
             *  Unlock the queue
             */
            xSemaphoreGive(WorkQueue->Lock);

            wi = CONTAINING_RECORD(Node, pvtWorkItem_t, Node);

            wi->Function(wi->UserData);

            free(wi);

            /**
             *  Lock the queue
             */
            xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);
        }

        /**
         *  Unlock the queue
         */
        xSemaphoreGive(WorkQueue->Lock);
    }
}



WorkQueue_t CreateWorkQueueEx(  const char * const Name,
                                uint16_t StackSize,
                                UBaseType_t Priority,
                                UBaseType_t maxWorkItems)
{
    pvtWorkQueue_t *wq = (pvtWorkQueue_t *)malloc(sizeof(pvtWorkQueue_t));
    
    if (wq == NULL)
        return NULL;

    InitQueue(&wq->Queue);

    wq->ThreadComplete = xSemaphoreCreateBinary();
    wq->QueueHasData = xSemaphoreCreateBinary();
    wq->Lock = xSemaphoreCreateMutex();

    BaseType_t rc = xTaskCreate(WqWorkerThread,
                                Name,
                                StackSize,
                                wq,
                                Priority,
                                &wq->WorkerThread);

    return wq;
}


#if (INCLUDE_vTaskDelete == 1)

void DestroyWorkQueue(WorkQueue_t WorkQueue)
{
}

#endif


int QueueWorkItem(WorkQueue_t WorkQueue, WorkItem_t WorkItem, void *UserData)
{
    /*********************/
    pvtWorkItem_t *wi;
    /*********************/

    wi = (pvtWorkItem_t *)malloc(sizeof(pvtWorkItem_t));

    wi->Function = WorkItem;
    wi->UserData = UserData;

    /**
     *  Lock the queue
     */
    xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);

    /**
     *  Put the work item on the queue.
     */
    Enqueue(&WorkQueue->Queue, wi->Node);
    
    /**
     *  Flag that the queue is non-empty.
     */
    xSemaphoreGive(WorkQueue->QueueHasData, portMAX_DELAY);

    /**
     *  Unlock the queue
     */
    xSemaphoreGive(WorkQueue->Lock);
}


