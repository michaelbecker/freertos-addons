/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
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

int numPoolItems1 = 10;
int numPoolItems2 = 10;
int numPoolItems3 = 10;
int numPoolItems4 = 10;
int numPoolItems5 = 10;
int numPoolItems6 = 10;
int numPoolItems7 = 10;
int numPoolItems8 = 10;
int numPoolItems9 = 10;


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

                StressPool(pool_1, numPoolItems1, 1);
                pool_1->AddMemory(PatternStart);
                numPoolItems1 += PatternStart;

                StressPool(pool_2, numPoolItems2, 2);
                pool_2->AddMemory(PatternStart);
                numPoolItems2 += PatternStart;

                StressPool(pool_3, numPoolItems3, 3);
                pool_3->AddMemory(PatternStart);
                numPoolItems3 += PatternStart;

                StressPool(pool_4, numPoolItems4, 4);
                pool_4->AddMemory(PatternStart);
                numPoolItems4 += PatternStart;

                StressPool(pool_5, numPoolItems5, 5);
                pool_5->AddMemory(PatternStart);
                numPoolItems5 += PatternStart;

                StressPool(pool_6, numPoolItems6, 6);
                pool_6->AddMemory(PatternStart);
                numPoolItems6 += PatternStart;

                StressPool(pool_7, numPoolItems7, 7);
                pool_7->AddMemory(PatternStart);
                numPoolItems7 += PatternStart;

                StressPool(pool_8, numPoolItems8, 8);
                pool_8->AddMemory(PatternStart);
                numPoolItems8 += PatternStart;

                StressPool(pool_9, numPoolItems9, 9);
                pool_9->AddMemory(PatternStart);
                numPoolItems9 += PatternStart;


                if (run_cnt++ > 5) {
                    cout << "suspending thread " << GetName() << endl;
                    Suspend();
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

    pool_1 = new MemoryPool(1, numPoolItems1, 1);
    pool_2 = new MemoryPool(2, numPoolItems2, 2);
    pool_3 = new MemoryPool(3, numPoolItems3, 4);
    pool_4 = new MemoryPool(4, numPoolItems4, 8);
    pool_5 = new MemoryPool(5, numPoolItems5, 16);
    pool_6 = new MemoryPool(6, numPoolItems6, 32);
    pool_7 = new MemoryPool(7, numPoolItems7, 64);
    pool_8 = new MemoryPool(8, numPoolItems8, 128);
    pool_9 = new MemoryPool(9, numPoolItems9, 256);

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


