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
#include "tasklet.hpp"


using namespace cpp_freertos;
using namespace std;



class TestTasklet : public Tasklet {

    public:
        TestTasklet(int id) :
            Tasklet(), Id(id) 
        {        
        };

        ~TestTasklet()
        {
            CheckForSafeDelete();
        }

    protected:
        virtual void Run(uint32_t parameter) {
            cout << "Running Tasket[" 
                 << Id << "](" << parameter << ")" << endl;
        };


    private:
        int Id;
};


class TestThread : public Thread {

    public:

        TestThread()
           : Thread("Thread", 100, 1)
        {
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << endl;
            TickType_t DelayInSeconds = 3;
            
            Tasklet *t[5];

            for (int i = 0; i < 5; i++) {
                t[i] = new TestTasklet(i);
            }
            
            int Parameter = 10;

            while (true) {

                Delay(Ticks::SecondsToTicks(DelayInSeconds));

                for (int i = 0; i < 5; i++) {
                    //
                    //  Given the Priority of the timer thread, these 
                    //  tasklets run as soon as they are scheduled.
                    //
                    cout << "Scheduling tasklet[" 
                         << i << "](" << Parameter << ")" << endl;
                    t[i]->Schedule(Parameter);
                }

                Parameter++;
            }
        };

    private:
        int id;
        int DelayInSeconds;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Tasklets" << endl;

    TestThread thread;

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


