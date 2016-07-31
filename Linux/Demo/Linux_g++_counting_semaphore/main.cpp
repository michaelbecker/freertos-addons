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
#include "semaphore.hpp"


using namespace cpp_freertos;
using namespace std;



class Producer : public Thread {

    public:

        Producer(Semaphore &sem)
           : Thread("Producer", 100, 1), Sem(sem)
        {
            Start();
        };

    protected:

        virtual void Run() {

            int SignaledCount = 0;

            cout << "[P] Starting Producer thread " << endl;
            
            TickType_t DelayInSeconds = 1; 

            while (true) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
                Delay(ticks);
                SignaledCount++;
                cout << "[P] Signaling work" << SignaledCount << " times" << endl;
                bool Success = Sem.Give();
                if (!Success) {
                    cout << "[P] Failed giving semaphore!" << endl;
                }
            }
        };

    private:
        Semaphore &Sem;
};


class Consumer : public Thread {

    public:

        Consumer(Semaphore &sem)
           : Thread("Consumer", 100, 1), Sem(sem)
        {
            Start();
        };

    protected:

        virtual void Run() {

            int SignaledCount = 0;
            
            while (true) {
                bool Success = Sem.Take();
                if (!Success) {
                    cout << "[C] Failed taking semaphore!" << endl;
                }
                SignaledCount++;
                cout << "[C] Consumer got work " << SignaledCount << " times" << endl;

                if ((SignaledCount % 5) == 0) {
                    cout << "[C] Simulating long work" << endl;
                    Delay(Ticks::SecondsToTicks(5));
                }
            }
        };

    private:
        Semaphore &Sem;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Counting Semaphore" << endl;

    // Set inital count lower to generate Sem,Give() failures
    CountingSemaphore Sem (10, 0);
    Producer sthd(Sem);
    Consumer wthr(Sem);

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


