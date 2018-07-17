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


/**
 *  Specify a struct to send information to different threads.
 */
struct thread_parameters {
    int id;
    int sleep_period;
};


/**
 *  For this example, we only create one task function and 
 *  pass it in slightly different parameters to change how it 
 *  runs. 
 *
 *  In the real world, there is almost always a single one-to-one
 *  correspondence between a task function and a task. Most embedded 
 *  systems don't run multiple copies of a task.
 */
void example_thread(void *parameters)
{
    struct thread_parameters *my_parameters;

    my_parameters = (struct thread_parameters *)parameters;

    printf("Thread #%d starting...\n", my_parameters->id);

    while(1) {

        vTaskDelay(my_parameters->sleep_period);

        printf("Thread #%d running\n", my_parameters->id);

        if (my_parameters->id == 3) {
            vTaskEndScheduler();
        }
    }

    configASSERT(!"CANNOT EXIT FROM A TASK");
}


int main (void)
{
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
    struct thread_parameters p1 = {1, 1000};
    struct thread_parameters p2 = {2, 2009};
    struct thread_parameters p3 = {3, 3017};


    printf("Simple test for FreeRTOS Linux port.\n");


    rc = xTaskCreate(   example_thread, 
                        "Task1",
                        stack_depth,
                        &p1,
                        1,
                        NULL);
    /**
     *  Make sure out task was created.
     */
    configASSERT(rc == pdPASS);


    rc = xTaskCreate(   example_thread, 
                        "Task2",
                        stack_depth,
                        &p2,
                        2,
                        NULL);
    /**
     *  Make sure out task was created.
     */
    configASSERT(rc == pdPASS);


    rc = xTaskCreate(   example_thread, 
                        "Task3",
                        stack_depth,
                        &p3,
                        3,
                        NULL);
    /**
     *  Make sure out task was created.
     */
    configASSERT(rc == pdPASS);


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


