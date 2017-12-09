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


