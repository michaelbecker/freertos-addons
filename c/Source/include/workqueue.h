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

