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
#include <cstring>
#include "thread.hpp"


using namespace cpp_freertos;


volatile bool Thread::SchedulerActive = false;
MutexStandard Thread::StartGuardLock;


//
//  We want to use C++ strings. This is the default.
//
#ifndef CPP_FREERTOS_NO_CPP_STRINGS

Thread::Thread( const std::string pcName,
                uint16_t usStackDepth,
                UBaseType_t uxPriority)
    :   Name(pcName), 
        StackDepth(usStackDepth), 
        Priority(uxPriority),
        ThreadStarted(false)
{
#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif
}


Thread::Thread( uint16_t usStackDepth,
                UBaseType_t uxPriority)
    :   Name("Default"), 
        StackDepth(usStackDepth), 
        Priority(uxPriority),
        ThreadStarted(false)
{
#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif
}

//
//  We do not want to use C++ strings. Fall back to character arrays.
//
#else

Thread::Thread( const char *pcName,
                uint16_t usStackDepth,
                UBaseType_t uxPriority)
    :   StackDepth(usStackDepth),
        Priority(uxPriority),
        ThreadStarted(false)
{
    for (int i = 0; i < configMAX_TASK_NAME_LEN - 1; i++) {
        Name[i] = *pcName;
        if (*pcName == 0)
            break;
        pcName++;
    }
    Name[configMAX_TASK_NAME_LEN - 1] = 0;

#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif
}


Thread::Thread( uint16_t usStackDepth,
                UBaseType_t uxPriority)
    :   StackDepth(usStackDepth),
        Priority(uxPriority),
        ThreadStarted(false)
{
    memset(Name, 0, sizeof(Name));
#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif
}

#endif


bool Thread::Start()
{
    //
    //  If the Scheduler is on, we need to lock before checking
    //  the ThreadStarted variable. We'll leverage the LockGuard 
    //  pattern, so we can create the guard and just forget it. 
    //  Leaving scope, including the return, will automatically 
    //  unlock it.
    //
    if (SchedulerActive) {

        LockGuard guard (StartGuardLock);

        if (ThreadStarted)
            return false;
        else 
            ThreadStarted = true;
    }
    //
    //  If the Scheduler isn't running, just check it.
    //
    else {

        if (ThreadStarted)
            return false;
        else 
            ThreadStarted = true;
    }

#ifndef CPP_FREERTOS_NO_CPP_STRINGS

    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                Name.c_str(),
                                StackDepth,
                                this,
                                Priority,
                                &handle);
#else 

    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                Name,
                                StackDepth,
                                this,
                                Priority,
                                &handle);
#endif

    return rc != pdPASS ? false : true;
}


#if (INCLUDE_vTaskDelete == 1)

//
//  Deliberately empty. If this is needed, it will be overloaded.
//
void Thread::Cleanup()
{
}


Thread::~Thread()
{
    vTaskDelete(handle);
    handle = (TaskHandle_t)-1;
}

#else

Thread::~Thread()
{
    configASSERT( ! "Cannot actually delete a thread object "
                    "if INCLUDE_vTaskDelete is not defined.");
}

#endif


void Thread::TaskFunctionAdapter(void *pvParameters)
{
    Thread *thread = static_cast<Thread *>(pvParameters);

    thread->Run();

#if (INCLUDE_vTaskDelete == 1)

    thread->Cleanup();

    vTaskDelete(thread->handle);

#else
    configASSERT( ! "Cannot return from a thread.run function "
                    "if INCLUDE_vTaskDelete is not defined.");
#endif
}


#if (INCLUDE_vTaskDelayUntil == 1)

void Thread::DelayUntil(const TickType_t Period)
{
    if (!delayUntilInitialized) {
        delayUntilInitialized = true;
        delayUntilPreviousWakeTime = xTaskGetTickCount();
    }

    vTaskDelayUntil(&delayUntilPreviousWakeTime, Period);
}


void Thread::ResetDelayUntil()
{
    delayUntilInitialized = false;
}

#endif



#ifdef CPP_FREERTOS_CONDITION_VARIABLES

bool Thread::Wait(  ConditionVariable &Cv,
                    Mutex &CvLock,
                    TickType_t Timeout)
{
    Cv.AddToWaitList(this);

    //
    //  Drop the associated external lock, as per cv semantics.
    //
    CvLock.Unlock();

    //
    //  And block on the internal semaphore. The associated Cv
    //  will call Thread::Signal, which will release the semaphore.
    //
    bool timed_out = ThreadWaitSem.Take(Timeout);
    
    //
    //  Grab the external lock again, as per cv semantics.
    //
    CvLock.Lock();

    return timed_out;
}


#endif


