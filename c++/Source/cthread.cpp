/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
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
 ***************************************************************************/
#include "thread.hpp"


using namespace cpp_freertos;


volatile bool Thread::SchedulerActive = false;
MutexStandard Thread::StartGuardLock;


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

    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                Name.c_str(),
                                StackDepth,
                                this,
                                Priority,
                                &handle);

    return rc != pdPASS ? false : true;
}


//
//  Deliberately empty. If this is needed, it will be overloaded.
//
void Thread::Cleanup()
{
}


#if (INCLUDE_vTaskDelete == 1)

Thread::~Thread()
{
    vTaskDelete(handle);
    handle = (TaskHandle_t)-1;
}

#endif


void Thread::TaskFunctionAdapter(void *pvParameters)
{
    Thread *thread = static_cast<Thread *>(pvParameters);

    thread->Run();

    thread->Cleanup();

#if (INCLUDE_vTaskDelete == 1)

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
