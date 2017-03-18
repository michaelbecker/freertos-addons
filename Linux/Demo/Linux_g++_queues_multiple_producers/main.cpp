/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
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
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C++ Wrappers project
 *  without being obliged to provide the source
 *  code for proprietary components.
 * 
 *  The FreeRTOS C++ Wrappers project GPL exception text follows:
 * 
 *  Any FreeRTOS C++ Wrapper *source code*, whether modified or in it's 
 *  original release form, or whether in whole or in part, can only be 
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C++ Wrappers project.
 * 
 *  Clause 1:
 * 
 *  Linking FreeRTOS C++ Wrappers project  with other modules is making a 
 *  combined work based on FreeRTOS C++ Wrappers project. Thus, the terms 
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 * 
 *  As a special exception, the copyright holders of FreeRTOS C++ Wrappers 
 *  project give you permission to link FreeRTOS C++ Wrappers project with 
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and 
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the 
 *  terms and conditions of the license of that module.  An independent 
 *  module is a module which is not derived from or based on FreeRTOS C++ 
 *  Wrappers project.
 * 
 *  Clause 2:
 * 
 *  FreeRTOS C++ Wrappers project may not be used for any competitive or 
 *  comparative purpose, including the publication of any form of run time 
 *  or compile time metric, without the express permission of the copyright 
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
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


