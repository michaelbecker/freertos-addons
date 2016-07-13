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
#include "idlehook.hpp"


using namespace cpp_freertos;
using namespace std;




class MyIdleHook : public IdleHook {

    public:
        MyIdleHook(int id) : IdleHook(), Id(id), Cnt(0)
        {
            cout << "Creating MyIdleHook " << Id << endl;
            Register();
        }

        virtual ~MyIdleHook() {
            cout << "Destroying MyIdleHook " << Id << endl;
        }

    protected:
        void Run() {

            //  
            //  Empirically determined, so we don't flood the console.
            //
            if (++Cnt > 1000000) {
                cout << "Running MyIdleHook # " << Id << endl;
                Cnt = 0;
            }
        }

    private:
        int Id;
        int Cnt;
};


class MyThread : public Thread {

    public:

        MyThread()
           : Thread("MyThread", 100, 1)
        {
        };

    protected:

        virtual void Run() {
            const int MAX_DYNAMIC_HOOKS = 20;
            int DelayInSeconds = 1;
            int Count = 0;
            int TickHookId = 3;
            MyIdleHook *DynamicHook[MAX_DYNAMIC_HOOKS];
            bool DeleteDynamicHook = false;

            cout << "Starting thread" << endl;
            
            while (true) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
                Delay(ticks);
                cout << "Running thread" << endl;

                if (++Count > 3) {

                    Count = 0;

                    if (DeleteDynamicHook) {
                        for (int i = 0; i < MAX_DYNAMIC_HOOKS; i++) {
                            //  
                            //  When we need to delete a hook, we call MarkForDelete(),
                            //  NOT delete DynamicHook (or we risk hitting a race 
                            //  condition and crashing.
                            //
                            DynamicHook[i]->MarkForDelete();
                            //
                            //  We maked it for deletion, we no longer own it.
                            //  so just forget about it,
                            //
                            DynamicHook[i] = NULL;
                            // 
                            //  Introduce a small delay to mix things up.
                            //
                            Delay(1);
                        }
                    }
                    else {
                        for (int i = 0; i < MAX_DYNAMIC_HOOKS; i++) {
                            DynamicHook[i] = new MyIdleHook(TickHookId++);
                        }
                    }

                    DeleteDynamicHook = !DeleteDynamicHook;
                }
            }
        };
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Idle Hooks" << endl;

    MyIdleHook hook1(1);
    MyIdleHook hook2(2);

    MyThread thr;

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


