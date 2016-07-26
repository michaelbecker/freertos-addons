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



class SignalingThread : public Thread {

    public:

        SignalingThread(Semaphore &sem)
           : Thread("SignalingThread", 100, 1), Sem(sem)
        {
        };

    protected:

        virtual void Run() {

            int SignaledCount = 0;

            cout << "[S] Starting Signaling thread " << endl;
            
            TickType_t DelayInSeconds = 1; 

            while (true) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
                Delay(ticks);
                SignaledCount++;
                cout << "[S] Signaling " << SignaledCount << " times" << endl;
                Sem.Give();
            }
        };

    private:
        Semaphore &Sem;
};


class WaitingThread : public Thread {

    public:

        WaitingThread(Semaphore &sem)
           : Thread("WaitingThread", 100, 1), Sem(sem)
        {
        };

    protected:

        virtual void Run() {

            int SignaledCount = 0;
            
            while (true) {
                Sem.Take();
                SignaledCount++;
                cout << "[W] Got Signal " << SignaledCount << " times" << endl;

                if ((SignaledCount % 5) == 0) {
                    cout << "[W] Simulating long work" << endl;
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
    cout << "Binary Semaphore" << endl;

    BinarySemaphore Sem;
    SignalingThread sthd(Sem);
    WaitingThread wthr(Sem);

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


