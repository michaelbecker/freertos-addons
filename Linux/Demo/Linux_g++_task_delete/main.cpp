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


using namespace cpp_freertos;
using namespace std;



class TestThread : public Thread {

    public:

        TestThread(int i, int delayInSeconds)
           : Thread("TestThread", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds)
        {
            Start();
        };

        ~TestThread() {
            cout << "dtor for Thread " << id << endl;
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << id << endl;
            
            for (int i = 0; i < 3; i++) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
            
                if (ticks)
                    Delay(ticks);
            
                cout << "Running thread " << id << endl;
            }

            cout << "Exiting Thread " << id << endl;
        };

    private:
        int id;
        int DelayInSeconds;
};


class TestThreadForever : public Thread {

    public:

        TestThreadForever(int i, int delayInSeconds)
           : Thread("TestThreadForever", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds)
        {
        };

        ~TestThreadForever() {
            cout << "dtor for TestThreadForever " << id << endl;
        };

    protected:

        virtual void Run() {

            cout << "Starting TestThreadForever " << id << endl;
            
            while (true) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
            
                if (ticks)
                    Delay(ticks);
            
                cout << "Running TestThreadForever " << id << endl;
            }
        };

    private:
        int id;
        int DelayInSeconds;
};


class TestThreadKiller : public Thread {

    public:

        TestThreadKiller(Thread *thr1, Thread *thr2)
           : Thread("KillerThread", 100, 1),
             Thread1(thr1), Thread2(thr2) 
        {
        };

        ~TestThreadKiller() {
            cout << "dtor for KillerThread" << endl;
        };

    protected:

        virtual void Run() {

            cout << "Starting Killer thread" << endl;
            
            TickType_t ticks = Ticks::SecondsToTicks(20);
            
            if (ticks)
                Delay(ticks);
            
            delete Thread1;
            delete Thread2;
        };

    private:
        Thread *Thread1; 
        Thread *Thread2;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Simple Tasks" << endl;

    TestThread thread1(1, 1);
    TestThread thread2(2, 2);
    TestThread thread3(3, 3);
    
    TestThreadForever *threadA = new TestThreadForever(5, 5);
    TestThreadForever *threadB = new TestThreadForever(6, 6);
    TestThreadKiller Killer(threadA, threadB);
    
    //
    //  Try variations of Start outside of the ctor.
    //  
    threadA->Start();
    threadB->Start();
    Killer.Start();

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


