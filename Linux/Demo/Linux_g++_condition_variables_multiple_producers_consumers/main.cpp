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
#include <list>
#include <vector>
#include "FreeRTOS.h"
#include "task.h"
#include "thread.hpp"
#include "ticks.hpp"
#include "mutex.hpp"
#include "condition_variable.hpp"


using namespace cpp_freertos;
using namespace std;


//
//  Simple implementation of a bounded queue, to demonstrate
//  how condition variables work. This is the classical
//  example for condition variables.
//
//  In this example, queues are _NOT_ thread safe and
//  cannot block. The whole point of condition variables in this
//  example is to use them to allow safe access and propegation
//  of execution when shared amongst threads.
//
class BoundedQueue {

    public:
        BoundedQueue(int max_size)
            : MaxSize(max_size), CurSize(0)
        {
        }

    void Add(int x)
    {
        CurSize++;
        configASSERT(CurSize <= MaxSize);
        Queue.push_front(x);
    }

    int Remove()
    {
        CurSize--;
        configASSERT(CurSize >= 0);
        int x = Queue.back();
        Queue.pop_back();
        return x;
    }

    bool IsEmpty()
    {
        if (CurSize == 0)
            return true;
        else 
            return false;
    }

    int IsFull()
    {
        if (CurSize == MaxSize)
            return true;
        else 
            return false;
    }

    private:
        int MaxSize;
        int CurSize;
        list<int>Queue;
};


BoundedQueue *boundedQueue;
MutexStandard boundedQueueLock;
ConditionVariable notEmptyCv;
ConditionVariable notFullCv;



class ProducerThread : public Thread {

    public:

        ProducerThread(string name, int data_start)
           : Thread(name, 100, 1), DataGenerator(data_start), runIterations(0)
        {
            //
            //  Now that construction is completed, we
            //  can safely start the thread.
            //  
            Start();
        };


        unsigned int GetRunIterations()
        {
            return runIterations;
        }

    protected:

        virtual void Run() {

            cerr << "Starting Producer thread " << GetName() << endl;
            
            while (true) {
            
                runIterations++;

                //
                //  We need to add a delay here to allow for the terminal
                //  to keep up, else we appear to deadlock inside our
                //  output statements.
                //
                if ((DataGenerator % 19) == 0)
                    Delay(100);

                boundedQueueLock.Lock();

                //cerr << GetName() << " queueing: " << DataGenerator << endl;
                
                while (boundedQueue->IsFull()) {
                    //cerr << GetName() << " - queue is full!, waiting..." << endl;
                    Wait(notFullCv, boundedQueueLock);
                }

                boundedQueue->Add(DataGenerator);
                DataGenerator++;

                notEmptyCv.Signal();

                boundedQueueLock.Unlock();
            }
        };

    private:
        int DataGenerator;
        volatile unsigned int runIterations;
};


class ConsumerThread : public Thread {

    public:

        ConsumerThread(string name, int delay_seed)
           : Thread(name, 100, 1), DelaySeed(delay_seed), runIterations(0)
        {
            //
            //  Now that construction is completed, we
            //  can safely start the thread.
            //  
            Start();
        };

        unsigned int GetRunIterations()
        {
            return runIterations;
        }
        

    protected:

        virtual void Run() {

            cerr << "Starting Consumer thread " << GetName() << endl;
            
            int cnt = 0;

            while (true) {

                runIterations++;

                //
                //  We need to add a delay here to allow for the terminal
                //  to keep up, else we appear to deadlock inside our
                //  output statements.
                //
                if ((++cnt % DelaySeed) == 0) {
                    Delay(100);
                }
                
                boundedQueueLock.Lock();
                
                while (boundedQueue->IsEmpty()) {
                    //cerr << GetName() << " - queue is empty!, waiting..." << endl;
                    Wait(notEmptyCv, boundedQueueLock);
                }

                int x = boundedQueue->Remove();
                (void)x;
                //cerr << GetName() << " dequeued: " << x << endl;

                notFullCv.Signal();

                boundedQueueLock.Unlock();
            }
        };

    private:

        const int DelaySeed;
        volatile unsigned int runIterations;
        
};



class MonitorThread : public Thread {

    public:

        MonitorThread(string name, vector<ConsumerThread *>cthr, vector<ProducerThread *>pthr)
           : Thread(name, 100, 2), ConsumerThreadList(cthr), ProducerThreadList(pthr)
        {
            //
            //  Now that construction is completed, we
            //  can safely start the thread.
            //  
            Start();
        };

    protected:

        virtual void Run() {

            cerr << "Starting Monitor thread " << GetName() << endl;
            
            while (true) {

                Delay(1000);
                
                cerr << "Thread runs:\n" << "--------------------------------" << endl;
                for (unsigned int i = 0; i < ConsumerThreadList.size(); i++) {
                    cerr << ConsumerThreadList[i]->GetName() << " : " 
                         << ConsumerThreadList[i]->GetRunIterations() << endl;
                }

                for (unsigned int i = 0; i < ProducerThreadList.size(); i++) {
                    cerr << ProducerThreadList[i]->GetName() << " : " 
                         << ProducerThreadList[i]->GetRunIterations() << endl;
                }
                cerr << endl;
            }
        };

    private:

        vector<ConsumerThread *>ConsumerThreadList;
        vector<ProducerThread *>ProducerThreadList;
};



int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Condition Variable - Bounded queue consumer / producer" << endl;


    boundedQueue = new BoundedQueue(1000);
    ProducerThread p1("Producer1", 1);
    ProducerThread p2("Producer2", 2);
    ProducerThread p3("Producer3", 3);
    ConsumerThread c1("Consumer1", 23);
    ConsumerThread c2("Consumer2", 27);
    ConsumerThread c3("Consumer3", 29);
    ConsumerThread c4("Consumer4", 31);

    vector<ConsumerThread *>clist;
    clist.push_back(&c1);
    clist.push_back(&c2);
    clist.push_back(&c3);
    clist.push_back(&c4);

    vector<ProducerThread *>plist;
    plist.push_back(&p1);
    plist.push_back(&p2);
    plist.push_back(&p3);

    MonitorThread monitor("Monitor", clist, plist);
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


