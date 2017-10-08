/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS-addons project.
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
 *  The FreeRTOS C++ Wrappers project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The FreeRTOS C++ Wrappers project is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the FreeRTOS C++ Wrappers project.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C++ Wrappers project
 *  without being obliged to provide the source
 *  code for proprietary components.
 * 
 *  The FreeRTOS C++ Wrappers project GPL exception text follows:
 * 
 *  Any FreeRTOS C++ Wrapper *source code*, whether modified or in it's 
 *  original release form, or whether in whole or in part, can only be 
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C++ Wrappers project.
 * 
 *  Clause 1:
 * 
 *  Linking FreeRTOS C++ Wrappers project  with other modules is making a 
 *  combined work based on FreeRTOS C++ Wrappers project. Thus, the terms 
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 * 
 *  As a special exception, the copyright holders of FreeRTOS C++ Wrappers 
 *  project give you permission to link FreeRTOS C++ Wrappers project with 
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and 
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the 
 *  terms and conditions of the license of that module.  An independent 
 *  module is a module which is not derived from or based on FreeRTOS C++ 
 *  Wrappers project.
 * 
 *  Clause 2:
 * 
 *  FreeRTOS C++ Wrappers project may not be used for any competitive or 
 *  comparative purpose, including the publication of any form of run time 
 *  or compile time metric, without the express permission of the copyright 
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
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
void StressPool(MemoryPool_t *p, int NumPoolItems, int dataSize, int PatternStart) 
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



MemoryPool_t *pool_1;
MemoryPool_t *pool_2;
MemoryPool_t *pool_3;
MemoryPool_t *pool_4;
MemoryPool_t *pool_5;
MemoryPool_t *pool_6;
MemoryPool_t *pool_7;
MemoryPool_t *pool_8;
MemoryPool_t *pool_9;


#define NUM_POOL_1_ITEMS    10
#define NUM_POOL_2_ITEMS    10
#define NUM_POOL_3_ITEMS    10
#define NUM_POOL_4_ITEMS    10
#define NUM_POOL_5_ITEMS    10
#define NUM_POOL_6_ITEMS    10
#define NUM_POOL_7_ITEMS    10
#define NUM_POOL_8_ITEMS    10
#define NUM_POOL_9_ITEMS    10

#define POOL_1_ITEM_SIZE    1
#define POOL_2_ITEM_SIZE    2
#define POOL_3_ITEM_SIZE    3
#define POOL_4_ITEM_SIZE    4
#define POOL_5_ITEM_SIZE    5
#define POOL_6_ITEM_SIZE    6
#define POOL_7_ITEM_SIZE    7
#define POOL_8_ITEM_SIZE    8
#define POOL_9_ITEM_SIZE    9

unsigned char Buffer1[NUM_POOL_1_ITEMS * POOL_1_ITEM_SIZE * 128];
unsigned char Buffer2[NUM_POOL_2_ITEMS * POOL_2_ITEM_SIZE * 128];
unsigned char Buffer3[NUM_POOL_3_ITEMS * POOL_3_ITEM_SIZE * 128];
unsigned char Buffer4[NUM_POOL_4_ITEMS * POOL_4_ITEM_SIZE * 128];
unsigned char Buffer5[NUM_POOL_5_ITEMS * POOL_5_ITEM_SIZE * 128];
unsigned char Buffer6[NUM_POOL_6_ITEMS * POOL_6_ITEM_SIZE * 128];
unsigned char Buffer7[NUM_POOL_7_ITEMS * POOL_7_ITEM_SIZE * 128];
unsigned char Buffer8[NUM_POOL_8_ITEMS * POOL_8_ITEM_SIZE * 128];
unsigned char Buffer9[NUM_POOL_9_ITEMS * POOL_9_ITEM_SIZE * 128];


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

    
    pool_1 = CreateMemoryPoolStatic(POOL_1_ITEM_SIZE, Buffer1, sizeof(Buffer1), 1);
    pool_2 = CreateMemoryPoolStatic(POOL_2_ITEM_SIZE, Buffer2, sizeof(Buffer2), 2);
    pool_3 = CreateMemoryPoolStatic(POOL_3_ITEM_SIZE, Buffer3, sizeof(Buffer3), 4);
    pool_4 = CreateMemoryPoolStatic(POOL_4_ITEM_SIZE, Buffer4, sizeof(Buffer4), 8);
    pool_5 = CreateMemoryPoolStatic(POOL_5_ITEM_SIZE, Buffer5, sizeof(Buffer5), 16);
    pool_6 = CreateMemoryPoolStatic(POOL_6_ITEM_SIZE, Buffer6, sizeof(Buffer6), 32);
    pool_7 = CreateMemoryPoolStatic(POOL_7_ITEM_SIZE, Buffer7, sizeof(Buffer7), 64);
    pool_8 = CreateMemoryPoolStatic(POOL_8_ITEM_SIZE, Buffer8, sizeof(Buffer8), 128);
    pool_9 = CreateMemoryPoolStatic(POOL_9_ITEM_SIZE, Buffer9, sizeof(Buffer9), 256);

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


