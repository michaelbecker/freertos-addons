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
#include "mem_pool.h"



/**
 *  Generic function to stress a memory pool.
 */
void StressPool(MemoryPool_t p, int NumPoolItems, int dataSize, int PatternStart) 
{
    unsigned char **addr;
    int localStart;
    int i, j;

    /*  Create an array to hold the addresses we will alloc. */
    addr = (unsigned char **)malloc(sizeof(unsigned char *) * (NumPoolItems + 1));

    /*  Set up the unique pattern(s) we are going to fill in. */
    localStart = PatternStart;

    /*  Try to allocate the whole pool. */
    for (i = 0; i < NumPoolItems + 1; i++) {
        
        addr[i] = (unsigned char*)MemoryPoolAllocate(p);
        
        /*  If you got an item, fill it with a known pattern, */
        /*  and create the next pattern. */
        if (addr[i]) {
            memset(addr[i], localStart, dataSize);
            localStart++;
        }
    }

    /*  Wait a bit just to allow multithreading to randomize things. */
    /*  We want the threads competing for the pools. */
    vTaskDelay(PatternStart);

    /*  Now check and free. */
    for (i = 0; i < NumPoolItems + 1; i++) {
        if (addr[i]) {
            /*  If we allocated an item from the pool, verify all  */
            /*  bytes are what we think they should be. */
            for (j = 0; j < dataSize; j++) {
                configASSERT(addr[i][j] == PatternStart);
            }
            /*  We are working with bytes so make sure we wrap. */
            if (++PatternStart >= 256) {
                PatternStart = 0;
            }
            /*  Poison the memory before freeing it. */
            memset(addr[i], 0xEE, dataSize);
            MemoryPoolFree(p, addr[i]);
            addr[i] = NULL;
        }
    }

    free(addr);
}


typedef struct ThreadParameters_t_ {
        
    int DelayInSeconds;
    int PatternStart;

} ThreadParameters_t;



MemoryPool_t pool_1;
MemoryPool_t pool_2;
MemoryPool_t pool_3;
MemoryPool_t pool_4;
MemoryPool_t pool_5;
MemoryPool_t pool_6;
MemoryPool_t pool_7;
MemoryPool_t pool_8;
MemoryPool_t pool_9;



void TestThread(void *parameters)
{
    ThreadParameters_t *tp;
    int TotalRuns = 0;

    tp = (ThreadParameters_t *)parameters;

    printf("Test Thread starting...\n");

    vTaskDelay(tp->DelayInSeconds * 100);

    while(1) {

        vTaskDelay(1);

        StressPool(pool_1, 10, 1, tp->PatternStart); 
        StressPool(pool_2, 10, 2, tp->PatternStart); 
        StressPool(pool_3, 10, 3, tp->PatternStart); 
        StressPool(pool_4, 10, 4, tp->PatternStart); 
        StressPool(pool_5, 10, 5, tp->PatternStart); 
        StressPool(pool_6, 10, 6, tp->PatternStart); 
        StressPool(pool_7, 10, 7, tp->PatternStart); 
        StressPool(pool_8, 10, 8, tp->PatternStart); 
        StressPool(pool_9, 10, 9, tp->PatternStart); 

        printf("running thread %d ...\n", tp->DelayInSeconds);
        TotalRuns++;
        if (TotalRuns > 20) {
            while (1) {
                vTaskDelay(10000);
            }
        }
    }

    configASSERT(!"CANNOT EXIT FROM A TASK");
}


int main (void)
{
    BaseType_t rc;
    int i;
    ThreadParameters_t params[5] = {
        {1, 1},
        {2, 3},
        {3, 5},
        {4, 7},
        {5, 11},
    };


    printf("Testing mem_pools\n");

    
    pool_1 = CreateMemoryPool(1, 10, 8);
    pool_2 = CreateMemoryPool(2, 10, 4);
    pool_3 = CreateMemoryPool(3, 10, 2);
    pool_4 = CreateMemoryPool(4, 10, 1);
    pool_5 = CreateMemoryPool(5, 10, 2);
    pool_6 = CreateMemoryPool(6, 10, 4);
    pool_7 = CreateMemoryPool(7, 10, 8);
    pool_8 = CreateMemoryPool(8, 10, 16);
    pool_9 = CreateMemoryPool(9, 10, 4);

    for (i = 0; i < 5; i++) {
        rc = xTaskCreate(   TestThread, 
                            "test",
                            1000,
                            (void *)&params[i],
                            3,
                            NULL);
        /**
         *  Make sure out task was created.
         */
        configASSERT(rc == pdPASS);
    }

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


