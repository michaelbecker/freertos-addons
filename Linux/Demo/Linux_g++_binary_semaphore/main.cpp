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
#include <iostream>
#include "FreeRTOS.h"
#include "task.h"
#include "thread.hpp"
#include "ticks.hpp"
#include "semaphore.hpp"


using namespace freertos;
using namespace std;



class SignalingThread : public Thread {

    public:

        SignalingThread(Semaphore &sem)
           : Thread("SignalingThread", 100, 1), Sem(sem)
        {
            Start();
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
            Start();
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

    BinarySemaphore *Sem;

    try {
        Sem = new BinarySemaphore();
    }
    catch(SemaphoreCreateException &ex) {
        cout << "Caught SemaphoreCreateException" << endl;
        cout << ex.what() << endl;
        configASSERT(!"BinarySemaphore creation failed!");
    }

    SignalingThread sthd(*Sem);
    WaitingThread wthr(*Sem);

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


