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
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "zero_copy_queue.h"



typedef struct TestData_t_ {

    int data[100];

}TestData_t;


#define TEST_DATA_COUNT 25


void CreateTestData(TestData_t *td, int start)
{
    int i;
    for (i = 0; i < 100; i++) {
        td->data[i] = start + i;
    }
}


int VerifyTestData(TestData_t *td, int start)
{
    int i;
    for (i = 0; i < 100; i++) {
        if (td->data[i] != (start + i))
            return 0;
    }
    return 1;
}


ZeroCopyQueue_t ZcQueue_1;


void SendTestThread(void *parameters)
{
    (void)parameters;
    TestData_t *td;
    int i = 0;
    int rc;

    printf("Sending Thread starting...\n");


    while(1) {

        td = (TestData_t *)ZcqAllocateItem(ZcQueue_1);
        if (td == NULL) {
            vTaskDelay(1000);
            td = (TestData_t *)ZcqAllocateItem(ZcQueue_1);
            configASSERT(td != NULL);
        }
        CreateTestData(td, i);

        rc = ZcqEnqueueItem(ZcQueue_1, td, 1000);

        configASSERT(rc == 1);
    
        i++;

        vTaskDelay(1);

        if (i > 10000){
            printf("%d messages sent ok - Done\n", i);
            vTaskDelay(100000);
        }
    }

    configASSERT(!"CANNOT EXIT FROM A TASK");
}


void ReceiveTestThread(void *parameters)
{
    (void)parameters;
    TestData_t *td;
    int i = 0;
    int rc;

    printf("Receiving Thread starting...\n");

    while(1) {

        td = ZcqDequeueItem(ZcQueue_1, 10000);
        
        configASSERT(td != NULL);

        rc = VerifyTestData(td, i);
        
        configASSERT(rc == 1);

        ZcqFreeItem(ZcQueue_1, td);

        i++;

        if (i % 1000 == 0)
            printf("%d messages received ok\n", i);

        if (i > 10000){
            printf("%d messages received ok - Done\n", i);
            vTaskDelay(100000);
        }
    }

    configASSERT(!"CANNOT EXIT FROM A TASK");
}


int main (void)
{
    BaseType_t rc;

    printf("Testing Zero copy queues\n");
    
    ZcQueue_1 = ZcqCreateQueue(sizeof(TestData_t), TEST_DATA_COUNT, 8);

    rc = xTaskCreate(   SendTestThread, 
                        "tx-test",
                        1000,
                        NULL,
                        3,
                        NULL);

    /**
     *  Make sure out task was created.
     */
    configASSERT(rc == pdPASS);

    rc = xTaskCreate(   ReceiveTestThread, 
                        "rx-test",
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


