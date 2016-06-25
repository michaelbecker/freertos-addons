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
        };

    protected:
        virtual void Run() {
            int iterationCount = 0;
            cout << "Starting thread " << id << endl;
            while (true) {
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
                if (ticks)
                    Delay(ticks);
                cout << "Running thread " << id << endl;
                if (++iterationCount > 10) {
                    cout << "Thread " << id << " ending scheduler" << endl;
                    EndScheduler();
                }
            }
        };

    private:
        int id;
        int DelayInSeconds;
};


int main (void)
{
    cout << "Running tests of FreeRTOS C++ wrappers" << endl;

    TestThread thread0(1, 1);
    TestThread thread1(2, 2);
    TestThread thread2(3, 3);

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
    while (1);
}


unsigned long ulGetRunTimeCounterValue(void)
{
    return 0;
}

void vConfigureTimerForRunTimeStats(void)
{
    return;
}

extern "C" void vApplicationTickHook(void);
void vApplicationTickHook(void)
{
    return;
}


volatile int IdleCount = 0;

extern "C" void vApplicationIdleHook(void);
void vApplicationIdleHook(void)
{
    IdleCount++;
}


extern "C" void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void)
{
	while(1);
}

