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
#include "read_write_lock.hpp"


using namespace cpp_freertos;
using namespace std;



class ReaderThread : public Thread {

    public:

        ReaderThread(int i, int delayInSeconds, ReadWriteLock &lock)
           : Thread("ReaderThread", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds),
             Lock(lock)
        {
            Start();
        };

    protected:

        virtual void Run() {

            while (true) {
            
                Delay(Ticks::SecondsToTicks(DelayInSeconds));

                Lock.ReaderLock();
                cout << "[ R "<< id << " ] Starting Read" << endl;
                Delay(Ticks::SecondsToTicks(3));
                cout << "[ R "<< id << " ] Ending Read" << endl;
                Lock.ReaderUnlock();
            }
        };

    private:
        int id;
        int DelayInSeconds;
        ReadWriteLock &Lock;
};


class WriterThread : public Thread {

    public:

        WriterThread(int i, int delayInSeconds, ReadWriteLock &lock)
           : Thread("WriterThread", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds),
             Lock(lock)
        {
            Start();
        };

    protected:

        virtual void Run() {

            while (true) {
            
                Delay(Ticks::SecondsToTicks(DelayInSeconds));

                Lock.WriterLock();
                cout << "[ W "<< id << " ] Starting Write ******** " << endl;
                Delay(Ticks::SecondsToTicks(2));
                cout << "[ W "<< id << " ] Ending Write ******** " << endl;
                Lock.WriterUnlock();
            }
        };

    private:
        int id;
        int DelayInSeconds;
        ReadWriteLock &Lock;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "ReadWriteLockPreferReader" << endl;

    ReadWriteLockPreferReader *Lock;

    try {
        Lock = new ReadWriteLockPreferReader();
    }
    catch(ReadWriteLockCreateException &ex) {
        cout << "Caught ReadWriteLockCreateException" << endl;
        cout << ex.what() << endl;
        configASSERT(!"ReadWriteLock creation failed!");
    }

    ReaderThread r1(1, 1, *Lock);
    ReaderThread r2(2, 1, *Lock);
    ReaderThread r3(3, 1, *Lock);
    ReaderThread r4(4, 5, *Lock);
    ReaderThread r5(5, 5, *Lock);

    WriterThread w1(10, 2, *Lock);
    WriterThread w2(11, 3, *Lock);


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


