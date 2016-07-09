/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
 *
 *  The FreeRTOS C++ Wrappers project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 3 of the License.
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


using namespace cpp_freertos;
using namespace std;



class TestThread : public Thread {

    public:

        TestThread()
           : Thread("TestThread", 100, 2)
        {
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << endl;

            int SimulatedWorkDelayMs = 0;
            int TaskPeriodMs = 500;
            TickType_t TaskPeriodTicks = Ticks::MsToTicks(TaskPeriodMs);
            
            while (true) {

                TickType_t Start = Ticks::GetTicks();

                SimulatedWorkDelayMs += 33;
                if (SimulatedWorkDelayMs > TaskPeriodMs) {
                    SimulatedWorkDelayMs = 0;
                    TaskPeriodMs += 500;
                    TaskPeriodTicks = Ticks::MsToTicks(TaskPeriodMs);
                }
                TickType_t ticks = Ticks::MsToTicks(SimulatedWorkDelayMs);

                //  
                //  Simulate variable length processing time
                //
                cout << "thread staring work for " << SimulatedWorkDelayMs << " ms ... ";
                Delay(ticks);
                cout << "\tCompleted" << endl;

                DelayUntil(TaskPeriodTicks);

                TickType_t End = Ticks::GetTicks();

                TickType_t Diff = End - Start;
                TickType_t PeriodMeasuredMs = Ticks::TicksToMs(Diff);

                cout << "Total Measured Thread Period: " << PeriodMeasuredMs << " ms." << endl;
            }
        };

    private:
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Delay Until Test" << endl;

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


