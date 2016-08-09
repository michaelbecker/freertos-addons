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


