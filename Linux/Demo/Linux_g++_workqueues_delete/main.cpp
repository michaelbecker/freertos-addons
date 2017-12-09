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
#include "workqueue.hpp"


using namespace cpp_freertos;
using namespace std;


class MyWorkItem : public WorkItem {

    public:
        MyWorkItem(int id, bool freeAfterComplete = false)
            : WorkItem(freeAfterComplete), Id(id)
        {
            WorkItemCount++;
        }

        ~MyWorkItem() 
        {
            WorkItemCount--;
        }

        void Run() 
        {
            cout << "[w:" << Id << "] " << WorkItemCount << " active." << endl;
        }

    private:
        int Id;
        static volatile int WorkItemCount;
};


volatile int MyWorkItem::WorkItemCount = 0;


class TestThread : public Thread {

    public:

        TestThread(int i, int delayInSeconds)
           : Thread("TestThread", 100, 3), 
             id (i), 
             DelayInSeconds(delayInSeconds),
             TotalWorkItems(0)
        {
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << id << endl;

            while (true) {
            
                Delay(Ticks::SecondsToTicks(DelayInSeconds));
                cout << "\n[t:" << id <<"] making work"<< endl;

                CreateWorkQueues();
                QueueWork();
                QueueExtraWork();
                DeleteWorkQueues();

                cout << "[t" << id <<"] done\n"<< endl;
            }
        };

    private:
        int id;
        int DelayInSeconds;
        WorkQueue *WqLow;
        WorkQueue *WqHigh;
        int TotalWorkItems;

        void CreateWorkQueues()
        {
            //
            //  Low priority work queue - lower than this thread
            //
            WqLow = new WorkQueue("WqLow", DEFAULT_WORK_QUEUE_STACK_SIZE, 1);

            //
            //  High priority work queue - higher than this thread
            //
            WqHigh = new WorkQueue("WqHigh", DEFAULT_WORK_QUEUE_STACK_SIZE, 5);
        }

        void DeleteWorkQueues()
        {
            delete WqLow;
            delete WqHigh;
        }

        void QueueWork()
        {
            MyWorkItem *work1;
            MyWorkItem *work2;
            
            work1 = new MyWorkItem(TotalWorkItems++, true);
            work2 = new MyWorkItem(TotalWorkItems++, true);

            WqLow->QueueWork(work1);
            WqHigh->QueueWork(work2);
        }

        void QueueExtraWork()
        {
            MyWorkItem *work1;
            
            work1 = new MyWorkItem(TotalWorkItems++, true);
            WqLow->QueueWork(work1);

            work1 = new MyWorkItem(TotalWorkItems++, true);
            WqLow->QueueWork(work1);

            work1 = new MyWorkItem(TotalWorkItems++, true);
            WqLow->QueueWork(work1);
        }
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Workqueues Delete" << endl;

    TestThread thread(1, 3);

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


