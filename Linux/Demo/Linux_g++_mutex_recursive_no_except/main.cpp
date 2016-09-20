/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
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
 ***************************************************************************/
#include <stdio.h>
#include <iostream>
#include "FreeRTOS.h"
#include "task.h"
#include "thread.hpp"
#include "ticks.hpp"
#include "mutex.hpp"


using namespace cpp_freertos;
using namespace std;



class TestThread : public Thread {

    public:

        TestThread(int id, int delayInMs, Mutex &lock)
           : Thread("TestThread", 100, 1), 
             Id (id), 
             DelayInMs(delayInMs),
             Lock(lock)
        {
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << Id << endl;
            
            while (true) {
            
                TickType_t ticks = Ticks::MsToTicks(DelayInMs);
                Delay(ticks);
                PrintLockedMessage();
            }
        };

    private:
        int Id;
        int DelayInMs;
        Mutex& Lock;

        // 
        //  Deliberately create a recursive locking pattern.
        //
        void PrintLockedMessage() {
            LockGuard guard(Lock);
            cout << "Printing message safely from ";
            PrintLockedThreadId();
        };

        void PrintLockedThreadId() {
            LockGuard guard(Lock);
            cout << "thread " << Id << endl;
        };
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Standard Mutexes" << endl;

    //  
    //  A standard mutex will deadlock in this example, 
    //  by design.
    //
    MutexRecursive *SharedLock;

    SharedLock = new MutexRecursive();

    TestThread thread0(1, 200, *SharedLock);
    TestThread thread1(2, 200, *SharedLock);
    TestThread thread2(3, 200, *SharedLock);

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


