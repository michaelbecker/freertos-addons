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


#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "workqueue.h"


void WorkItemFunction(void *parameter)
{
    int id = (intptr_t)parameter;
    printf("[w:%d] active\n", id);
}


void TestThread(void *parameters)
{
    WorkQueue_t lowPriWorkQueue;
    WorkQueue_t highPriWorkQueue;
    int rc;
    int lowCount;
    int highCount;

    (void)parameters;
    printf("Test Thread starting...\n");

    lowCount = 0;
    highCount = 1000;

    printf("[t] creating queues\n");
    
    lowPriWorkQueue = CreateWorkQueue();
    configASSERT(lowPriWorkQueue != NULL);

    highPriWorkQueue = CreateWorkQueueEx(   "wq", 
                                            DEFAULT_WORK_QUEUE_STACK_SIZE,
                                            6);
    configASSERT(highPriWorkQueue != NULL);

    while(1) {

        rc = QueueWorkItem(lowPriWorkQueue, WorkItemFunction, (void *)(intptr_t)lowCount++);
        configASSERT(rc == pdPASS);

        rc = QueueWorkItem(highPriWorkQueue, WorkItemFunction, (void *)(intptr_t)highCount++);
        configASSERT(rc == pdPASS);

        rc = QueueWorkItem(lowPriWorkQueue, WorkItemFunction, (void *)(intptr_t)lowCount++);
        configASSERT(rc == pdPASS);

        rc = QueueWorkItem(highPriWorkQueue, WorkItemFunction, (void *)(intptr_t)highCount++);
        configASSERT(rc == pdPASS);
    
        rc = QueueWorkItem(lowPriWorkQueue, WorkItemFunction, (void *)(intptr_t)lowCount++);
        configASSERT(rc == pdPASS);

        rc = QueueWorkItem(highPriWorkQueue, WorkItemFunction, (void *)(intptr_t)highCount++);
        configASSERT(rc == pdPASS);

        rc = QueueWorkItem(lowPriWorkQueue, WorkItemFunction, (void *)(intptr_t)lowCount++);
        configASSERT(rc == pdPASS);

        rc = QueueWorkItem(highPriWorkQueue, WorkItemFunction, (void *)(intptr_t)highCount++);
        configASSERT(rc == pdPASS);

        printf("[t] pausing\n");
        vTaskDelay(500);
    }

    configASSERT(!"CANNOT EXIT FROM A TASK");
}


int main (void)
{
    BaseType_t rc;

    printf("Testing Work Queues\n");

    rc = xTaskCreate(   TestThread, 
                        "test",
                        1000,
                        NULL,
                        3,
                        NULL);
    /**
     *  Make sure out task was created.
     */
    configASSERT(rc == pdPASS);


    /**
     *  Start FreeRTOS here.
     */
    vTaskStartScheduler();

    /*
     *  We shouldn't ever get here unless someone calls 
     *  vTaskEndScheduler(). Note that there appears to be a 
     *  bug in the Linux FreeRTOS simulator that crashes when
     *  this is called.
     */
    printf("Scheduler ended!\n");

    return 0;
}


void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    printf("ASSERT: %s : %d\n", pcFileName, (int)ulLine);
    while(1);
}


unsigned long ulGetRunTimeCounterValue(void)
{
    return 0;
}

void vConfigureTimerForRunTimeStats(void)
{
    return;
}


void vApplicationMallocFailedHook(void)
{
	while(1);
}


