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
#include "freertos/thread.hpp"
#include "freertos/ticks.hpp"
#include "freertos/queue.hpp"


using namespace freertos;
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
    Queue *MessageQueue;

    try {
        MessageQueue = new Queue(5, sizeof(int));
    }
    catch(QueueCreateException &ex) {
        cout << "Caught QueueCreateException" << endl;
        cout << ex.what() << endl;
        configASSERT(!"Queue creation failed!");
    }

    ProducerThread p1(1, 1, 3, *MessageQueue);
    ProducerThread p2(2, 1, 3, *MessageQueue);
    ProducerThread p3(3, 1, 3, *MessageQueue);
    ProducerThread p4(4, 1, 3, *MessageQueue);
    ProducerThread p5(5, 1, 3, *MessageQueue);

    ConsumerThread c1(1, 0, *MessageQueue);

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


