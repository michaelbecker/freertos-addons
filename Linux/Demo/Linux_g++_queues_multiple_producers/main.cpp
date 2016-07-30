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
#include "queue.hpp"


using namespace cpp_freertos;
using namespace std;


class ProducerThread : public Thread {

    public:

        ProducerThread(int i, int delayInSeconds, int burstAmount, Queue &q)
           : Thread("ProducerThread", 100, 1), 
             Id (i), 
             DelayInSeconds(delayInSeconds),
             BurstAmount(burstAmount),
             MessageQueue(q)
        {
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting ProducerThread " << Id << endl;
            int Message = Id * 100;

            while (true) {
                
                Delay(Ticks::SecondsToTicks(DelayInSeconds));
                for (int i = 0; i < BurstAmount; i++) {
                    cout << "[P" << Id << "] Sending Message: " << Message << endl;
                    MessageQueue.Enqueue(&Message);
                    Message++;
                }
            }
        };

    private:
        int Id;
        int DelayInSeconds;
        int BurstAmount;
        Queue &MessageQueue;
};


class ConsumerThread : public Thread {

    public:

        ConsumerThread(int i, int delayInSeconds, Queue &q)
           : Thread("ConsumerThread", 100, 1), 
             Id (i), 
             DelayInSeconds(delayInSeconds),
             MessageQueue(q)
        {
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting ConsumerThread " << Id << endl;
            int Message;

            while (true) {
                
                MessageQueue.Dequeue(&Message);
                cout << "[C" << Id << "] Received Message: " << Message << endl;
                Delay(Ticks::SecondsToTicks(DelayInSeconds));
            }
        };

    private:
        int Id;
        int DelayInSeconds;
        Queue &MessageQueue;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Queues Simple Producer / Consumer" << endl;

    //
    //  These parameters may be adjusted to explore queue 
    //  behaviors.
    //
    Queue MessageQueue(5, sizeof(int));

    ProducerThread p1(1, 1, 3, MessageQueue);
    ProducerThread p2(2, 1, 3, MessageQueue);
    ProducerThread p3(3, 1, 3, MessageQueue);
    ProducerThread p4(4, 1, 3, MessageQueue);
    ProducerThread p5(5, 1, 3, MessageQueue);

    ConsumerThread c1(1, 0, MessageQueue);

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


