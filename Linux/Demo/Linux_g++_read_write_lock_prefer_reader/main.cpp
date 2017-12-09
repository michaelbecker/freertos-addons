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
#include "read_write_lock.hpp"


using namespace cpp_freertos;
using namespace std;



class ReaderThread : public Thread {

    public:

        ReaderThread(int i, int delayInSeconds, ReadWriteLock &lock)
           : Thread("ReaderThread", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds),
             Lock(lock)
        {
            Start();
        };

    protected:

        virtual void Run() {

            while (true) {
            
                Delay(Ticks::SecondsToTicks(DelayInSeconds));

                Lock.ReaderLock();
                cout << "[ R "<< id << " ] Starting Read" << endl;
                Delay(Ticks::SecondsToTicks(3));
                cout << "[ R "<< id << " ] Ending Read" << endl;
                Lock.ReaderUnlock();
            }
        };

    private:
        int id;
        int DelayInSeconds;
        ReadWriteLock &Lock;
};


class WriterThread : public Thread {

    public:

        WriterThread(int i, int delayInSeconds, ReadWriteLock &lock)
           : Thread("WriterThread", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds),
             Lock(lock)
        {
            Start();
        };

    protected:

        virtual void Run() {

            while (true) {
            
                Delay(Ticks::SecondsToTicks(DelayInSeconds));

                Lock.WriterLock();
                cout << "[ W "<< id << " ] Starting Write ******** " << endl;
                Delay(Ticks::SecondsToTicks(2));
                cout << "[ W "<< id << " ] Ending Write ******** " << endl;
                Lock.WriterUnlock();
            }
        };

    private:
        int id;
        int DelayInSeconds;
        ReadWriteLock &Lock;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "ReadWriteLockPreferReader" << endl;

    ReadWriteLockPreferReader *Lock;

    try {
        Lock = new ReadWriteLockPreferReader();
    }
    catch(ReadWriteLockCreateException &ex) {
        cout << "Caught ReadWriteLockCreateException" << endl;
        cout << ex.what() << endl;
        configASSERT(!"ReadWriteLock creation failed!");
    }

    ReaderThread r1(1, 1, *Lock);
    ReaderThread r2(2, 1, *Lock);
    ReaderThread r3(3, 1, *Lock);
    ReaderThread r4(4, 5, *Lock);
    ReaderThread r5(5, 5, *Lock);

    WriterThread w1(10, 2, *Lock);
    WriterThread w2(11, 3, *Lock);


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


