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
#include <cstring>
#include <iostream>
#include "FreeRTOS.h"
#include "task.h"
#include "thread.hpp"
#include "ticks.hpp"
#include "mem_pool.hpp"


using namespace cpp_freertos;
using namespace std;


MemoryPool *pool_1;
MemoryPool *pool_2;
MemoryPool *pool_3;
MemoryPool *pool_4;
MemoryPool *pool_5;
MemoryPool *pool_6;
MemoryPool *pool_7;
MemoryPool *pool_8;
MemoryPool *pool_9;

#define NUM_POOL_1_ITEMS    10
#define NUM_POOL_2_ITEMS    10
#define NUM_POOL_3_ITEMS    10
#define NUM_POOL_4_ITEMS    10
#define NUM_POOL_5_ITEMS    10
#define NUM_POOL_6_ITEMS    10
#define NUM_POOL_7_ITEMS    10
#define NUM_POOL_8_ITEMS    10
#define NUM_POOL_9_ITEMS    10


class TestThread : public Thread {

    public:

        TestThread(string name, int delayInSeconds, int patternStart)
           : Thread(name, 100, 1),
             DelayInSeconds(delayInSeconds),
             PatternStart(patternStart)
        {
            //
            //  Now that construction is completed, we
            //  can safely start the thread.
            //  
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << GetName() << endl;
            TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
            Delay(ticks);

            int run_cnt = 0;

            while (true) {
            
                ticks = Ticks::MsToTicks(1);
                Delay(ticks);

                StressPool(pool_1, NUM_POOL_1_ITEMS, 1);
                StressPool(pool_2, NUM_POOL_2_ITEMS, 2);
                StressPool(pool_3, NUM_POOL_3_ITEMS, 3);
                StressPool(pool_4, NUM_POOL_4_ITEMS, 4);
                StressPool(pool_5, NUM_POOL_5_ITEMS, 5);
                StressPool(pool_6, NUM_POOL_6_ITEMS, 6);
                StressPool(pool_7, NUM_POOL_7_ITEMS, 7);
                StressPool(pool_8, NUM_POOL_8_ITEMS, 8);
                StressPool(pool_9, NUM_POOL_9_ITEMS, 9);

                if (run_cnt++ > 1) {
                    run_cnt = 0;
                    cout << "Running thread " << GetName() << endl;
                }
            }
        };

    private:
        int DelayInSeconds;
        int PatternStart;

        //
        //  Generic function to stress a memory pool.
        //
        void StressPool(MemoryPool *p, int NumPoolItems, int dataSize) {

            //
            //  Create an array to hold the addresses we will alloc.
            //
            unsigned char **addr = new unsigned char *[NumPoolItems + 1];

            //
            //  Set up the unique pattern(s) we are going to fill in.
            //
            int localStart = PatternStart;

            //
            //  Try to allocate the whole pool.
            //
            for (int i = 0; i < NumPoolItems + 1; i++) {
                
                addr[i] = (unsigned char*)p->Allocate();
                
                //
                //  If you got an item, fill it with a known pattern,
                //  and create the next pattern.
                //
                if (addr[i]) {
                    memset(addr[i], localStart, dataSize);
                    localStart++;
                }
            }

            //
            //  Wait a bit just to allow multithreading to randomize things.
            //  We want the threads competing for the pools.
            //
            TickType_t ticks = Ticks::MsToTicks(PatternStart);
            Delay(ticks);

            //
            //  Now check and free.
            //
            for (int i = 0; i < NumPoolItems + 1; i++) {
                if (addr[i]) {
                    //
                    //  If we allocated an item from the pool, verify all 
                    //  bytes are what we think they should be.
                    //
                    for (int j = 0; j < dataSize; j++) {
                        configASSERT(addr[i][j] == PatternStart);
                    }
                    //
                    //  We are working with bytes so make sure we wrap.
                    //
                    if (++PatternStart >= 256) {
                        PatternStart = 0;
                    }
                    //
                    //  Poison the memory before freeing it.
                    //
                    memset(addr[i], 0xEE, dataSize);
                    p->Free(addr[i]);
                    addr[i] = NULL;
                }
            }

            delete [] addr;
        }
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "MemoryPool Testing - Run this under Valgrind" << endl;

    TestThread thread1("Thread_1", 1, 1);
    TestThread thread2("Thread_2", 1, 3);
    TestThread thread3("Thread_3", 1, 5);
    TestThread thread4("Thread_4", 1, 7);
    TestThread thread5("Thread_5", 1, 11);

    pool_1 = new MemoryPool(1, NUM_POOL_1_ITEMS, 4);
    pool_2 = new MemoryPool(2, NUM_POOL_2_ITEMS, 8);
    pool_3 = new MemoryPool(3, NUM_POOL_3_ITEMS, 2);
    pool_4 = new MemoryPool(4, NUM_POOL_4_ITEMS, 16);
    pool_5 = new MemoryPool(5, NUM_POOL_5_ITEMS, 32);
    pool_6 = new MemoryPool(6, NUM_POOL_6_ITEMS, 64);
    pool_7 = new MemoryPool(7, NUM_POOL_7_ITEMS, 128);
    pool_8 = new MemoryPool(8, NUM_POOL_8_ITEMS, 256);
    pool_9 = new MemoryPool(9, NUM_POOL_9_ITEMS, 512);

    Thread::StartScheduler();

    //
    //  We shouldn't ever get here unless someone calls 
    //  Thread::EndScheduler()
    //

    cout << "Scheduler ended!" << endl;

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


extern "C" void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void)
{
	while(1);
}


