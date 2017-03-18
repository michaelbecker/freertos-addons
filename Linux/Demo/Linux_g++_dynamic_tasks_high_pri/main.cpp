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


using namespace cpp_freertos;
using namespace std;



class DynamicThread : public Thread {

    public:

        DynamicThread(string name, int delayInSeconds)
           : Thread(name, 100, 3), 
             DelayInSeconds(delayInSeconds)
        {
            cout  << GetName() << " ctor" << endl;
        };

        ~DynamicThread()
        {
            cout << GetName() << " dtor" << endl;
        }

        virtual void Cleanup()
        {
            delete this;
        }

    protected:

        virtual void Run() {

            cout  << GetName() << " starting" << endl;
            
            int count = 0;

            while (true) {
            
                Delay(Ticks::SecondsToTicks(DelayInSeconds));
                cout << GetName() << " running " << count <<  endl;

                //
                //  Just leave the loop. The wrapper code will 
                //  handle calling the dtor.
                //
                if (++count > 3) {
                    break;
                }
            }
        };

    private:
        int DelayInSeconds;
};



class RootThread : public Thread {

    public:

        RootThread(string name, int delayInSeconds)
           : Thread(name, 100, 2), 
             DelayInSeconds(delayInSeconds)
        {
            //
            //  Now that construction is completed, we
            //  can safely start the thread.
            //  
            Start();
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << GetName() << endl;
            
            int count = 0;
            string threadName;
            DynamicThread *dt;
            int threadCnt = 0;

            while (true) {
            
                Delay(Ticks::SecondsToTicks(DelayInSeconds));

                count++;

                switch (count) {
                    case 1:
                    case 2:
                        break;

                    case 3: {
                        char nameBuf[10];
                        sprintf(nameBuf, "dy%d", ++threadCnt);
                        cout << GetName() << " Creating " << nameBuf << endl;
                        dt = new DynamicThread(nameBuf, 1);
                        }
                        break;

                    case 4:
                        cout << GetName() << " Starting " << threadName << endl;
                        dt->Start();
                        break;

                    default:
                        count = 0;
                        break;
                }
            }
        };

    private:
        int DelayInSeconds;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Dynamic Tasks" << endl;

    RootThread rootThread("RT", 1);

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


