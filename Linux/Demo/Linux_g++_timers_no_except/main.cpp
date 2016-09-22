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
#include "timer.hpp"


using namespace cpp_freertos;
using namespace std;


class PeriodicTimer : public Timer {

    public:
        PeriodicTimer(int id, TickType_t PeriodInTicks) 
            : Timer(PeriodInTicks), Id(id)
        {
        };

    protected:

        virtual void Run() {
            cout << "[#T " << Id << " ] : Periodic" << endl;
        };

    private:
        int Id;

};


class SingleRunTimer : public Timer {

    public:
        SingleRunTimer(int id, TickType_t PeriodInTicks) 
            : Timer(PeriodInTicks, false), Id(id)
        {
        };

    protected:

        virtual void Run() {
            cout << "[=> " << Id << " ] : One Shot" << endl;
        };

    private:
        int Id;

};


class TestThread : public Thread {

    public:

        TestThread(PeriodicTimer &t, SingleRunTimer &s)
           : Thread("TestThread", 100, 1), periodicTimer(t), singleRunTimer(s)
        {
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << endl;
            int State = 0;
            
            while (true) {
                TickType_t DelayInSeconds = 10;
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
                Delay(ticks);

                cout << "Changing Timer States" << endl;

                switch (State) {

                    case 0:
                        cout << "periodicTimer.Stop()" << endl;
                        cout << "singleRunTimer.Start(1 sec)" << endl;
                        periodicTimer.Stop();
                        singleRunTimer.SetPeriod(Ticks::SecondsToTicks(1));
                        singleRunTimer.Start();
                        State++;
                        break;

                    case 1:
                        cout << "periodicTimer.Reset()" << endl;
                        cout << "singleRunTimer.Start(2 sec)" << endl;
                        periodicTimer.Reset();
                        singleRunTimer.SetPeriod(Ticks::SecondsToTicks(2));
                        singleRunTimer.Start();
                        State++;
                        break;

                    case 2:
                        cout << "periodicTimer.Stop()" << endl;
                        periodicTimer.Stop();
                        State++;
                        break;

                    default:
                    case 3:
                        cout << "periodicTimer.Start()" << endl;
                        periodicTimer.Start();
                        State = 0;
                        break;
                }
            }
        };

    private:
        PeriodicTimer &periodicTimer;
        SingleRunTimer &singleRunTimer;
};



class ResetTimerThread : public Thread {

    public:

        ResetTimerThread(PeriodicTimer &t)
           : Thread("ResetTimerThread", 100, 1), periodicTimer(t)
        {
        };

    protected:

        virtual void Run() {

            cout << "Starting ResetTimerThread " << endl;

            TickType_t DelayInSeconds = 10;
            TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
            Delay(ticks);
            
            cout << "Resetting Timer once / second. "
                    "This should prevent the timer from running" << endl;

            while (true) {
                periodicTimer.Reset();
                ticks = Ticks::SecondsToTicks(1);
                Delay(ticks);
            }
        };

    private:
        PeriodicTimer &periodicTimer;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Timers" << endl;

    PeriodicTimer t1(1, Ticks::SecondsToTicks(1));
    t1.Start();

    PeriodicTimer t2(2, Ticks::SecondsToTicks(2));
    t2.Start();

    SingleRunTimer s1(1, Ticks::SecondsToTicks(3));
    s1.Start();

    TestThread thr1(t1, s1);
    ResetTimerThread thr2(t2);

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


