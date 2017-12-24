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
#include "task.h"
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


/**
 *  The internal WorkQueue data structure.
 */
typedef struct pvtWorkQueue_t_ {

    /**
     *  The actual queue.
     */
    Queue_t Queue;

    /**
     *  Wake the work queue thread up, something interesting happened.
     */
    SemaphoreHandle_t Event;

    SemaphoreHandle_t Lock;

    TaskHandle_t WorkerThread;

#if (INCLUDE_vTaskDelete == 1)
    /**
     *  Flag if we are tearing down the WorkQueue.
     */    
    int ExitThread;

#endif

} pvtWorkQueue_t;



static void WorkerThread(void *parameters)
{
    /****************************************/
    pvtWorkQueue_t *WorkQueue;
    DlNode_t *Node;
    pvtWorkItem_t *WorkItem;

#if (INCLUDE_vTaskDelete == 1)

    TaskHandle_t handle;
    int ExitThread;

#endif
    /****************************************/


    WorkQueue = (pvtWorkQueue_t*)parameters;


    while (1) {

        /**
         *  Wait to be woken.
         */
        xSemaphoreTake(WorkQueue->Event, portMAX_DELAY);

        /**
         *  Lock the queue
         */
        xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);

        /**
         *  Keep looping until the work items are all done.
         */
        while ( !IsQueueEmpty(&WorkQueue->Queue)) {
            /**
             *  Dequeue an item.
             */
            Node = Dequeue(&WorkQueue->Queue);

            /**
             *  Unlock the queue, the lock is really only for 
             *  the Queue struct.
             */
            xSemaphoreGive(WorkQueue->Lock);

            /**
             *  Recover the actual work item pointer.
             */
            WorkItem = CONTAINING_RECORD(Node, pvtWorkItem_t, Node);

            /**
             *  And call the function.
             */
            WorkItem->Function(WorkItem->UserData);

            /**
             *  All done, free it.
             */
            free(WorkItem);

            /**
             *  Lock the queue again so we can check if the 
             *  Queue is empty yet.
             */
            xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);
        }
    
#if (INCLUDE_vTaskDelete == 1)
        /**
         *  Are we being asked to exit? We want to do this while
         *  holding the lock.
         */
        ExitThread = WorkQueue->ExitThread;

#endif
        /**
         *  Unlock now
         */
        xSemaphoreGive(WorkQueue->Lock);

#if (INCLUDE_vTaskDelete == 1)
        /**
         *  If we need to exit, then jump out of the task loop.
         */
        if (ExitThread) {
            break;
        }

#endif
    }

#if (INCLUDE_vTaskDelete == 1)
    
    /**
     *  Thread cleanup here.
     */

    /**
     *  Lock this work queue.
     */
    xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);

    /**
     *  Drain the queue and free everything.
     */
    while ( !IsQueueEmpty(&WorkQueue->Queue)) {
        
        /**
         *  Dequeue the node
         */
        Node = Dequeue(&WorkQueue->Queue);

        /**
         *  Recover the work item from the node.
         */
        WorkItem = CONTAINING_RECORD(Node, pvtWorkItem_t, Node);

        /**
         *  And free it.
         */
        free(WorkItem);
    }

    /**
     *  Finally unlock
     */
    xSemaphoreGive(WorkQueue->Lock);

    /**
     *  Free the semaphores
     */
    vSemaphoreDelete(WorkQueue->Lock);
    vSemaphoreDelete(WorkQueue->Event);

    /**
     *  We need to save a local copy of our handle. 
     */
    handle = WorkQueue->WorkerThread;

    /**
     *  And free the work queue stat structure itself.
     */
    free(WorkQueue);

    /**
     *  Finally delete ourselves, we won't go past this call.
     */
    vTaskDelete(handle);

#endif    
}


WorkQueue_t CreateWorkQueueEx(  const char * const Name,
                                uint16_t StackSize,
                                UBaseType_t Priority)
{
    /****************************/
    pvtWorkQueue_t *WorkQueue;
    BaseType_t rc;
    /****************************/
    
    WorkQueue = (pvtWorkQueue_t *)malloc(sizeof(pvtWorkQueue_t));

    if (WorkQueue == NULL)
        return NULL;

    InitQueue(&WorkQueue->Queue);

    WorkQueue->Event = xSemaphoreCreateBinary();

    if (WorkQueue->Event == NULL) {
        free(WorkQueue);
        return NULL;
    }

    WorkQueue->Lock = xSemaphoreCreateMutex();

    if (WorkQueue->Lock == NULL) {
        vSemaphoreDelete(WorkQueue->Event);
        free(WorkQueue);
        return NULL;
    }

#if (INCLUDE_vTaskDelete == 1)
 
    WorkQueue->ExitThread = 0;

#endif

    rc = xTaskCreate(   WorkerThread,
                        Name,
                        StackSize,
                        WorkQueue,
                        Priority,
                        &WorkQueue->WorkerThread);

    if (rc != pdPASS) {
        vSemaphoreDelete(WorkQueue->Lock);
        vSemaphoreDelete(WorkQueue->Event);
        free(WorkQueue);
        return NULL;
    }

    return WorkQueue;
}


#if (INCLUDE_vTaskDelete == 1)

void DestroyWorkQueue(WorkQueue_t wq)
{
    /****************************/
    pvtWorkQueue_t *WorkQueue;
    /****************************/

    WorkQueue = (pvtWorkQueue_t *)wq;

    /**
     *  Lock
     */
    xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);

    /**
     *  Flag we are all done.
     */
    WorkQueue->ExitThread = 1;
    
    /**
     *  Signal the Worker thead.
     */
    xSemaphoreGive(WorkQueue->Event);

    /**
     *  Unlock
     */
    xSemaphoreGive(WorkQueue->Lock);
}

#endif


int QueueWorkItem(WorkQueue_t wq, WorkItem_t Function, void *UserData)
{
    /****************************/
    pvtWorkQueue_t *WorkQueue;
    pvtWorkItem_t *WorkItem;
    /****************************/

    WorkQueue = (pvtWorkQueue_t *)wq;

    WorkItem = (pvtWorkItem_t *)malloc(sizeof(pvtWorkItem_t));
    if (WorkItem == NULL) {
        return pdFAIL;
    }

    WorkItem->Function = Function;
    WorkItem->UserData = UserData;

    /**
     *  Lock the queue
     */
    xSemaphoreTake(WorkQueue->Lock, portMAX_DELAY);

    /**
     *  Put the work item on the queue.
     */
    Enqueue(&WorkQueue->Queue, &WorkItem->Node);
    
    /**
     *  Wake the Worker thread up.
     */
    xSemaphoreGive(WorkQueue->Event);

    /**
     *  Unlock the queue
     */
    xSemaphoreGive(WorkQueue->Lock);

    return pdPASS;
}


