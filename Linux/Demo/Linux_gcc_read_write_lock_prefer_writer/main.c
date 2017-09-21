/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS-addons project.
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
        vTaskDelay(2000);
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
        vTaskDelay(1000);
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

    struct thread_parameters w[2] = { {10, 6000}, {11, 7000} };


    printf("Testing Reader / Writer locks - prefering reader\n");


    Lock = CreateReadWriteLockPreferWriter();


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


