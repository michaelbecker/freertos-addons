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


#ifndef WORKQUEUE_H_
#define WORKQUEUE_H_


#include "FreeRTOS.h"
#include "semphr.h"


/**
 *  All your work items are actually functions that take a 
 *  void * parameter.
 */
typedef void (* WorkItem_t)(void *UserData);


/**
 *  Handle for the actual work queue.
 */
typedef void * WorkQueue_t;


/**
 *  Default stack size of the Worker Task.
 */
#define DEFAULT_WORK_QUEUE_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)


/**
 *  Default task priority of the Worker Task.
 */
#define DEFAULT_WORK_QUEUE_PRIORITY     (tskIDLE_PRIORITY + 1)


/**
 *  Create a WorkQueue, specifying all options.
 *
 *  @param Name The name of the worker thread.
 *  @param StackSize The size of the worker thread stack, in words.
 *  @param Priority The priority of the worker thread.
 *  @return A handle, or NULL on error.
 */
WorkQueue_t CreateWorkQueueEx(  const char * const Name,
                                uint16_t StackSize,
                                UBaseType_t Priority);


/**
 *  Create a WorkQueue using the defaults.
 *
 *  @return A handle, or NULL on error.
 */
#define CreateWorkQueue()                   \
    CreateWorkQueueEx("wq",                 \
            DEFAULT_WORK_QUEUE_STACK_SIZE,  \
            DEFAULT_WORK_QUEUE_PRIORITY)    \


#if (INCLUDE_vTaskDelete == 1)

/**
 *  Destroy a WorkQueue, if allowed.
 *
 *  @param WorkQueue The work queue.
 */
void DestroyWorkQueue(WorkQueue_t WorkQueue);

#endif

/**
 *  Add an item of work onto the queue.
 *
 *  @param WorkQueue The work queue.
 *  @param WorkItem The function you want called.
 *  @param UserData A value passed back to you.
 *  @return pdPASS on success, pdFAIL on error.
 */
int QueueWorkItem(  WorkQueue_t WorkQueue, 
                    WorkItem_t WorkItem, 
                    void *UserData);


#endif

