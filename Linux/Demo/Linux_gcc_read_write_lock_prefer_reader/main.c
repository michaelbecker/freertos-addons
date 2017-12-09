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
#include "FreeRTOS.h"
#include "task.h"
#include "read_write_lock.h"


/**
 *  Specify a struct to send information to different threads.
 */
struct thread_parameters {
    int id;
    int delay;
};


ReadWriteLock_t *Lock;


void ReaderThread(void *parameters)
{
    struct thread_parameters *my_parameters;
    my_parameters = (struct thread_parameters *)parameters;
    printf("Reader #%d starting...\n", my_parameters->id);

    while(1) {

        vTaskDelay(my_parameters->delay);

        ReaderLock(Lock);
        printf("[ R %d ] starting read\n", my_parameters->id);
        vTaskDelay(1000);
        printf("[ R %d ] ending read\n", my_parameters->id);
        ReaderUnlock(Lock);
    }

    configASSERT(!"CANNOT EXIT FROM A TASK");
}


void WriterThread(void *parameters)
{
    struct thread_parameters *my_parameters;
    my_parameters = (struct thread_parameters *)parameters;
    printf("Writer #%d starting...\n", my_parameters->id);

    while(1) {

        vTaskDelay(my_parameters->delay);
        
        WriterLock(Lock);
        printf("[ W %d ] starting write\n", my_parameters->id);
        vTaskDelay(2000);
        printf("[ W %d ] ending write\n", my_parameters->id);
        WriterUnlock(Lock);
    }

    configASSERT(!"CANNOT EXIT FROM A TASK");
}


int main (void)
{
    int i;
    BaseType_t rc;
    const uint16_t stack_depth = 1000;

    /**
     *  We are passing pointers to these structs for the tasks to use.
     *  Note that the storage for these is actually on the main() stack.
     *  Usually this is a "bad" idea, passing pointers from one task 
     *  callstack to another task, however in this case we know that 
     *  main() will effectively be blocked forever unless we actually 
     *  choose to exit from FreeRTOS, so as long as we aren't too clever 
     *  and try and reclaim this stack memory, this works ok.
     */
    struct thread_parameters r[5] = {   {1, 1000},
                                        {2, 1000},
                                        {3, 1000},
                                        {4, 5000},
                                        {5, 5000} };

    struct thread_parameters w[2] = { {10, 2000}, {11, 3000} };


    printf("Testing Reader / Writer locks - prefering reader\n");


    Lock = CreateReadWriteLockPreferReader();


    for (i = 0; i < 5; i++) {
        rc = xTaskCreate(   ReaderThread, 
                            "Reader",
                            stack_depth,
                            &r[i],
                            1,
                            NULL);
        /**
         *  Make sure out task was created.
         */
        configASSERT(rc == pdPASS);
    }


    for (i = 0; i < 2; i++) {
        rc = xTaskCreate(   WriterThread, 
                            "Writer",
                            stack_depth,
                            &w[i],
                            1,
                            NULL);
        /**
         *  Make sure out task was created.
         */
        configASSERT(rc == pdPASS);
    }

    /**
     *  Start FreeRTOS here.
     */
    vTaskStartScheduler();

    /*
     *  We shouldn't ever get here unless someone calls 
     *  vTaskEndScheduler(). Note that there appears to be a 
     *  bug in the Linux FreeRTOS simulator that crashes when
     *  this is called.
     */
    printf("Scheduler ended!\n");

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


void vApplicationMallocFailedHook(void)
{
	while(1);
}


